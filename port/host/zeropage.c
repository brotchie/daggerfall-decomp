/* zeropage.c: DOS's low memory under a null pointer (port_host.h). Under the DOS extender the
   game's flat address space starts at linear 0, so a read through a pointer not set yet (or an
   empty slot: an enemy faction, a flat FLATS.CFG does not list) reaches the real-mode
   interrupt table and the BIOS data area, and the game goes on with what it finds there. In
   the native build address 0 is unmapped: the access faults. This handler finishes such an
   access against the virtual PC's low memory (port_low_memory_block, whose first KB is zeros as
   in tools/fallemu.py) and resumes after the instruction, so the game sees what DOS gave it.

   It decodes the arm64 general-register loads and stores clang emits for C: LDR/STR (the
   unsigned-offset, unscaled, register-offset, pre- and post-indexed forms, every size and
   sign extension) and LDP/STP. Anything else (a SIMD load, an address past low memory) is a
   real fault. Each place is reported once: the sources mark the known ones with DOS_NULL
   (include/doslow.h), so a report is a site to look at. PORT_ZERO_PAGE=0 turns it off. */
#include <dlfcn.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>

#include "port_host.h"
#include "port_vpc.h"

#define MAX_SITES 256

static uint64_t sites[MAX_SITES];
static int n_sites;
static int enabled = -1;

static uint64_t get_x(struct __darwin_arm_thread_state64 *ss, unsigned r)
{
    if (r < 29)
        return ss->__x[r];
    if (r == 29)
        return ss->__fp;
    if (r == 30)
        return ss->__lr;
    return 0;                           /* XZR (as a data register) */
}

static void set_x(struct __darwin_arm_thread_state64 *ss, unsigned r, uint64_t v)
{
    if (r < 29)
        ss->__x[r] = v;
    else if (r == 29)
        ss->__fp = v;
    else if (r == 30)
        ss->__lr = v;
}

/* a base register: 31 is SP there */
static void set_base(struct __darwin_arm_thread_state64 *ss, unsigned r, uint64_t v)
{
    if (r == 31)
        ss->__sp = v;
    else
        set_x(ss, r, v);
}

static uint64_t low_read(uint64_t addr, unsigned size)
{
    uint64_t v = 0;
    unsigned i;

    for (i = 0; i < size; i++)
        v |= (uint64_t)port_low_memory_block[addr + i] << (8 * i);
    return v;
}

static void low_write(uint64_t addr, unsigned size, uint64_t v)
{
    unsigned i;

    for (i = 0; i < size; i++)
        port_low_memory_block[addr + i] = (unsigned char)(v >> (8 * i));
}

static uint64_t extend(uint64_t v, unsigned size, int sign, int to64)
{
    if (sign && size < 8 && (v >> (size * 8 - 1)) & 1)
        v |= ~0ull << (size * 8);
    if (!to64)
        v &= 0xFFFFFFFFull;
    return v;
}

static void report(uint64_t pc, uint64_t addr, int store, unsigned size)
{
    Dl_info info;
    int i;

    for (i = 0; i < n_sites; i++)
        if (sites[i] == pc)
            return;
    if (n_sites < MAX_SITES)
        sites[n_sites++] = pc;
    if (dladdr((void *)pc, &info) && info.dli_sname)
        fprintf(stderr, "port: zero page %s of %u at 0x%llX in %s+%llu\n", store ? "write" : "read",
                size, (unsigned long long)addr, info.dli_sname,
                (unsigned long long)(pc - (uint64_t)info.dli_saddr));
    else
        fprintf(stderr, "port: zero page %s of %u at 0x%llX, pc 0x%llX\n", store ? "write" : "read",
                size, (unsigned long long)addr, (unsigned long long)pc);
}

int port_zero_page_fault(void *siginfo, void *ucontext)
{
    siginfo_t *si = siginfo;
    ucontext_t *uc = ucontext;
    struct __darwin_arm_thread_state64 *ss;
    uint64_t addr, pc;
    uint32_t insn;
    unsigned rt, rn, size;

    if (enabled < 0) {
        const char *e = getenv("PORT_ZERO_PAGE");
        enabled = !(e != NULL && strcmp(e, "0") == 0);
    }
    if (!enabled || si == NULL || uc == NULL)
        return 0;
    addr = (uint64_t)si->si_addr;
    if (addr >= VPC_LOWMEM_SIZE)
        return 0;
    ss = &uc->uc_mcontext->__ss;
    pc = __darwin_arm_thread_state64_get_pc(*ss);
    insn = *(const uint32_t *)pc;
    rt = insn & 31;
    rn = (insn >> 5) & 31;

    if ((insn & 0x3A000000) == 0x38000000 && !(insn & 0x04000000)) {
        /* LDR/STR (general registers): size bits 31:30, opc bits 23:22 */
        unsigned opc = (insn >> 22) & 3;
        int store = opc == 0, writeback = 0;
        uint64_t base_after = 0;

        size = 1u << (insn >> 30);
        if ((insn & 0x01000000) == 0) {
            unsigned kind = (insn >> 10) & 3;
            if (insn & 0x00200000) {
                if (kind != 2)
                    return 0;           /* (atomics share the space) */
            } else if (kind == 1 || kind == 3) {
                int64_t imm9 = (int64_t)((int32_t)(insn << 11) >> 23);
                writeback = 1;
                base_after = kind == 3 ? addr : addr + (uint64_t)imm9;
            } else if (kind == 2) {
                return 0;               /* unprivileged forms */
            }
        }
        if (addr + size > VPC_LOWMEM_SIZE)
            return 0;
        if (size == 8 && opc == 2)
            ;                           /* PRFM: nothing to do */
        else if (store)
            low_write(addr, size, get_x(ss, rt));
        else if (opc == 1)
            set_x(ss, rt, extend(low_read(addr, size), size, 0, size == 8));
        else if (opc == 2)
            set_x(ss, rt, extend(low_read(addr, size), size, 1, 1));       /* LDRS* to X */
        else
            set_x(ss, rt, extend(low_read(addr, size), size, 1, 0));       /* LDRS* to W */
        if (writeback)
            set_base(ss, rn, base_after);
        report(pc, addr, store, size);
    } else if ((insn & 0x3A000000) == 0x28000000 && !(insn & 0x04000000)) {
        /* LDP/STP: opc bits 31:30 (00 W, 01 LDPSW, 10 X), L bit 22, index bits 24:23 */
        unsigned opc = insn >> 30, rt2 = (insn >> 10) & 31, idx = (insn >> 23) & 3;
        int load = (insn >> 22) & 1;
        int64_t imm7 = (int64_t)((int32_t)(insn << 10) >> 25);

        if (opc == 3 || idx == 0)
            return 0;
        size = opc == 2 ? 8 : 4;
        imm7 *= size;
        if (addr + 2 * size > VPC_LOWMEM_SIZE)
            return 0;
        if (load) {
            uint64_t a = low_read(addr, size), b = low_read(addr + size, size);
            set_x(ss, rt, extend(a, size, opc == 1, opc != 0));
            set_x(ss, rt2, extend(b, size, opc == 1, opc != 0));
        } else {
            low_write(addr, size, get_x(ss, rt));
            low_write(addr + size, size, get_x(ss, rt2));
        }
        if (idx == 1)
            set_base(ss, rn, addr + (uint64_t)imm7);    /* post-index */
        else if (idx == 3)
            set_base(ss, rn, addr);                     /* pre-index */
        report(pc, addr, !load, 2 * size);
    } else {
        return 0;
    }
    __darwin_arm_thread_state64_set_pc_fptr(*ss, (void *)(pc + 4));
    return 1;
}
