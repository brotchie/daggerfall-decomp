#!/usr/bin/env python3
"""The int services XnGine and the game call: which registers each takes and defines.

tools/xn_abi.py reads it for liveness; tools/xn_record.py's replay reads it so that a recorded
service gives back only the registers the service defines. The others keep what the replayed
code holds, which matters when C runs instead of the asm: a register that holds a stack
address across the int (lseek's EDI) differs from the asm run's.
"""

# int services: (vector, ah or ax) -> (inputs, outputs always written, outputs maybe written)
# Only what the engine calls; unknown services use every register.
SERVICES = {
    (0x21, "ah", 0x09): ("eax edx", "", "eax"),
    (0x21, "ah", 0x1A): ("eax edx", "", ""),
    (0x21, "ah", 0x25): ("eax edx ds", "", ""),
    (0x21, "ah", 0x2A): ("eax", "eax ecx edx", ""),
    (0x21, "ah", 0x2C): ("eax", "ecx edx", ""),
    (0x21, "ah", 0x30): ("eax", "eax", "ebx ecx"),
    (0x21, "ah", 0x35): ("eax", "ebx es", ""),
    (0x21, "ah", 0x36): ("eax edx", "eax ebx ecx edx", ""),
    (0x21, "ah", 0x39): ("eax edx", "", "eax CF"),
    (0x21, "ah", 0x3A): ("eax edx", "", "eax CF"),
    (0x21, "ah", 0x3B): ("eax edx", "", "eax CF"),
    (0x21, "ah", 0x3C): ("eax ecx edx", "eax CF", ""),
    (0x21, "ah", 0x3D): ("eax edx", "eax CF", ""),
    (0x21, "ah", 0x3E): ("eax ebx", "CF", "eax"),
    (0x21, "ah", 0x3F): ("eax ebx ecx edx", "eax CF", ""),
    (0x21, "ah", 0x40): ("eax ebx ecx edx", "eax CF", ""),
    (0x21, "ah", 0x41): ("eax edx", "CF", "eax"),
    (0x21, "ah", 0x42): ("eax ebx ecx edx", "eax edx CF", ""),
    (0x21, "ah", 0x43): ("eax ecx edx", "CF", "eax ecx"),
    (0x21, "ah", 0x44): ("eax ebx ecx edx", "CF", "eax edx"),
    (0x21, "ah", 0x47): ("eax edx esi", "CF", "eax"),
    (0x21, "ah", 0x48): ("eax ebx", "eax CF", "ebx"),
    (0x21, "ah", 0x49): ("eax es", "CF", "eax"),
    (0x21, "ah", 0x4A): ("eax ebx es", "CF", "eax ebx"),
    (0x21, "ah", 0x4C): ("eax", "", ""),
    (0x21, "ah", 0x4E): ("eax ecx edx", "eax CF", ""),
    (0x21, "ah", 0x4F): ("eax", "eax CF", ""),
    (0x21, "ah", 0x56): ("eax edx edi es", "CF", "eax"),
    (0x21, "ah", 0x57): ("eax ebx ecx edx", "CF", "eax ecx edx"),
    (0x31, "ax", 0x0000): ("eax ecx", "eax CF", ""),
    (0x31, "ax", 0x0001): ("eax ebx", "CF", "eax"),
    (0x31, "ax", 0x0002): ("eax ebx", "eax CF", ""),
    (0x31, "ax", 0x0003): ("eax", "eax", ""),
    (0x31, "ax", 0x0006): ("eax ebx", "ecx edx CF", "eax"),
    (0x31, "ax", 0x0007): ("eax ebx ecx edx", "CF", "eax"),
    (0x31, "ax", 0x0008): ("eax ebx ecx edx", "CF", "eax"),
    (0x31, "ax", 0x0009): ("eax ebx ecx", "CF", "eax"),
    (0x31, "ax", 0x000B): ("eax ebx edi es", "CF", "eax"),
    (0x31, "ax", 0x000C): ("eax ebx edi es", "CF", "eax"),
    (0x31, "ax", 0x0100): ("eax ebx", "eax edx CF", "ebx"),
    (0x31, "ax", 0x0101): ("eax edx", "CF", "eax"),
    (0x31, "ax", 0x0200): ("eax ebx", "ecx edx CF", "eax"),
    (0x31, "ax", 0x0201): ("eax ebx ecx edx", "CF", "eax"),
    (0x31, "ax", 0x0202): ("eax ebx", "ecx edx CF", "eax"),
    (0x31, "ax", 0x0203): ("eax ebx ecx edx", "CF", "eax"),
    (0x31, "ax", 0x0204): ("eax ebx", "ecx edx CF", "eax"),
    (0x31, "ax", 0x0205): ("eax ebx ecx edx", "CF", "eax"),
    (0x31, "ax", 0x0300): ("eax ebx ecx edi es", "CF", "eax"),
    (0x31, "ax", 0x0301): ("eax ebx ecx edi es", "CF", "eax"),
    (0x31, "ax", 0x0302): ("eax ebx ecx edi es", "CF", "eax"),
    (0x31, "ax", 0x0500): ("eax edi es", "CF", "eax"),
    (0x31, "ax", 0x0501): ("eax ebx ecx", "ebx ecx esi edi CF", "eax"),
    (0x31, "ax", 0x0502): ("eax esi edi", "CF", "eax"),
    (0x31, "ax", 0x0503): ("eax ebx ecx esi edi", "ebx ecx esi edi CF", "eax"),
    (0x31, "ax", 0x0600): ("eax ebx ecx esi edi", "CF", "eax"),
    (0x31, "ax", 0x0601): ("eax ebx ecx esi edi", "CF", "eax"),
    (0x31, "ax", 0x0800): ("eax ebx ecx esi edi", "ebx ecx CF", "eax"),
    (0x31, "ax", 0x0801): ("eax ebx ecx", "CF", "eax"),
    (0x33, "ax", 0x0000): ("eax", "eax ebx", ""),
    (0x33, "ax", 0x0001): ("eax", "", ""),
    (0x33, "ax", 0x0002): ("eax", "", ""),
    (0x33, "ax", 0x0003): ("eax", "ebx ecx edx", ""),
    (0x33, "ax", 0x0004): ("eax ecx edx", "", ""),
    (0x33, "ax", 0x0007): ("eax ecx edx", "", ""),
    (0x33, "ax", 0x0008): ("eax ecx edx", "", ""),
    (0x33, "ax", 0x000B): ("eax", "ecx edx", ""),
    (0x33, "ax", 0x000C): ("eax ecx edx es", "", ""),
    (0x33, "ax", 0x000F): ("eax ecx edx", "", ""),
    (0x33, "ax", 0x0014): ("eax ecx edx es", "ecx edx es", ""),
    (0x33, "ax", 0x001A): ("eax ebx ecx edx", "", ""),
    (0x33, "ax", 0x0024): ("eax", "ebx ecx", ""),
    (0x33, "ax", 0x001B): ("eax", "ebx ecx edx", ""),       # mouse sensitivity
    (0x2F, "ax", 0x1680): ("eax", "", "eax"),                # release the time slice (AL = 0)
    (0x10, "ah", 0x00): ("eax", "", "eax"),
    (0x10, "ah", 0x0F): ("eax", "eax ebx", ""),
    (0x10, "ax", 0x1012): ("eax ebx ecx edx es", "", ""),
    (0x10, "ax", 0x1017): ("eax ebx ecx edx es", "", ""),
    (0x16, "ah", 0x00): ("eax", "eax", ""),
    (0x16, "ah", 0x01): ("eax", "ZF", "eax"),
    (0x16, "ah", 0x02): ("eax", "eax", ""),
    (0x16, "ah", 0x10): ("eax", "eax", ""),
    (0x16, "ah", 0x11): ("eax", "ZF", "eax"),
}


# The services whose AL is not an input (the DOS and BIOS references): a call of one is
# compared by AH alone (eax_mask), since AL is whatever its caller left there (`mov ah, N`),
# which canonical C keeps differently from the asm. Checked against tools/fallemu.py's own
# implementations (int21, int16, int10): none of them reads AL for these.
AH_ONLY = {
    0x21: {
        0x09: "print a '$'-terminated string",
        0x1A: "set the disk transfer area",
        0x2A: "get the date",
        0x2C: "get the time",
        0x36: "free disk space (DL the drive)",
        0x39: "make a directory",
        0x3A: "remove a directory",
        0x3B: "change the directory",
        0x3C: "create a file (CX the attributes)",
        0x3E: "close a file",
        0x3F: "read from a file",
        0x40: "write to a file",
        0x41: "delete a file",
        0x47: "get the current directory",
        0x48: "allocate memory",
        0x49: "free memory",
        0x4A: "resize memory",
        0x4E: "find the first file (CX the attributes)",
        0x4F: "find the next file",
        0x51: "get the PSP",
        0x56: "rename a file",
    },
    0x16: {
        0x00: "read a key",
        0x01: "is a key waiting",
        0x02: "the shift flags",
        0x10: "read a key (enhanced)",
        0x11: "is a key waiting (enhanced)",
    },
    0x10: {
        0x02: "set the cursor position",
        0x03: "get the cursor position",
        0x0F: "get the video mode",
    },
}


def eax_mask(vector, eax):
    """The bits of EAX a call of service (vector, eax) is compared by: AH for the services of
    AH_ONLY, AX for the rest (every service the engine and the game call takes its function in
    AH or AX: EAX's upper half is the caller's)."""
    return 0xFF00 if ((eax >> 8) & 0xFF) in AH_ONLY.get(vector, ()) else 0xFFFF


def replay_regs(vector, eax):
    """The registers and flags (names) a recorded service call writes back on replay: its
    must- and may-define sets; None (every register) for a service not listed."""
    for key in ((vector, "ax", eax & 0xFFFF), (vector, "ah", (eax >> 8) & 0xFF)):
        s = SERVICES.get(key)
        if s:
            return set((s[1] + " " + s[2]).split())
    return None
