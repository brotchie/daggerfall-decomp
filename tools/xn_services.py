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


def replay_regs(vector, eax):
    """The registers and flags (names) a recorded service call writes back on replay: its
    must- and may-define sets; None (every register) for a service not listed."""
    for key in ((vector, "ax", eax & 0xFFFF), (vector, "ah", (eax >> 8) & 0xFF)):
        s = SERVICES.get(key)
        if s:
            return set((s[1] + " " + s[2]).split())
    return None
