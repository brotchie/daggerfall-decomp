#!/usr/bin/env python3
"""Watchdog: kill this repo's emulator processes before the computer runs out of memory.

Each emulator process holds 0.5-1.5 GB, and a dozen of them once took a 36 GB Mac to a
watchdog panic (the kernel's memory compressor full, the machine thrashing until it gave up).
This checks the kernel's own measure of free memory (kern.memorystatus_level, the "memory
free percentage" of memory_pressure) and swap in use every couple of seconds; when free
memory is low, it kills the largest process running a tool of this repo (tools/*.py), one at
a time, until there is room again. Only this repo's processes are touched.

The tools that start many emulators (fallfuzz.py run, fallevidence.py collect, fallcov.py run)
start it in the background themselves (start()). Run it on its own while agents play:

usage: memwatch.py [--min-free PCT] [--max-swap GB] [--interval S]
"""
import argparse
import ctypes
import os
import re
import signal
import subprocess
import sys
import threading
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TOOLS = os.path.join(ROOT, "tools")
LOG = os.path.join(ROOT, "build", "memwatch.log")
MIN_FREE = 12           # percent: kill below this
MAX_SWAP = 6.0          # GB of swap in use: kill above this (the disk is small, and swapping
                        # emulators is no faster than rerunning them)

_libc = ctypes.CDLL(None)


def _sysctl(name, ctype):
    v = ctype()
    n = ctypes.c_size_t(ctypes.sizeof(v))
    if _libc.sysctlbyname(name.encode(), ctypes.byref(v), ctypes.byref(n), None, 0) != 0:
        return None
    return v


class _Swap(ctypes.Structure):          # struct xsw_usage
    _fields_ = [("total", ctypes.c_uint64), ("avail", ctypes.c_uint64),
                ("used", ctypes.c_uint64), ("pagesize", ctypes.c_uint32),
                ("encrypted", ctypes.c_uint32)]


def free_pct():
    """The kernel's memory free percentage (100 when it can't be read, e.g. not macOS)."""
    v = _sysctl("kern.memorystatus_level", ctypes.c_int)
    return v.value if v is not None else 100


def swap_gb():
    v = _sysctl("vm.swapusage", _Swap)
    return v.used / (1 << 30) if v is not None else 0.0


def ours():
    """[(rss MB, pid, command)] of the processes running this repo's tools, largest first."""
    try:
        out = subprocess.check_output(["ps", "-Ao", "pid=,rss=,command="], text=True)
    except (OSError, subprocess.CalledProcessError):
        return []
    me = {os.getpid()}
    rows = []
    for line in out.splitlines():
        w = line.split(None, 2)
        if len(w) < 3 or int(w[0]) in me:
            continue
        cmd = w[2]
        if "python" in os.path.basename(cmd.split()[0]) and re.search(r"(^|[\s/])tools/(fall|xn_)\w*\.py", cmd.replace(ROOT + "/", "")):
            rows.append((int(w[1]) / 1024, int(w[0]), cmd))
    return sorted(rows, reverse=True)


def log(msg):
    line = "%s memwatch: %s" % (time.strftime("%H:%M:%S"), msg)
    print(line, file=sys.stderr, flush=True)
    try:
        os.makedirs(os.path.dirname(LOG), exist_ok=True)
        with open(LOG, "a") as f:
            f.write(line + "\n")
    except OSError:
        pass


def check(min_free=MIN_FREE, max_swap=MAX_SWAP, spare=()):
    """One look; kills one process if memory is short. Returns the pid killed, or None.
    spare: pids never to kill (a coordinator whose workers should go first)."""
    f, s = free_pct(), swap_gb()
    if f >= min_free and s <= max_swap:
        return None
    victims = [r for r in ours() if r[1] not in spare]
    if not victims:
        return None
    mb, pid, cmd = victims[0]
    log("memory free %d%%, swap %.1f GB: killing pid %d (%.0f MB): %s" % (f, s, pid, mb, cmd[:160]))
    try:
        os.kill(pid, signal.SIGTERM)
        for _ in range(20):
            time.sleep(0.1)
            os.kill(pid, 0)
        os.kill(pid, signal.SIGKILL)
    except ProcessLookupError:
        pass
    except PermissionError:
        return None
    return pid


def start(min_free=MIN_FREE, max_swap=MAX_SWAP, interval=2.0):
    """Watch in a background thread of this process (sparing it: its workers go first)."""
    def loop():
        while True:
            if check(min_free, max_swap, spare={os.getpid()}) is not None:
                time.sleep(3)               # let the memory come back before looking again
            time.sleep(interval)
    t = threading.Thread(target=loop, name="memwatch", daemon=True)
    t.start()
    return t


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min-free", type=int, default=MIN_FREE, help="percent (default %d)" % MIN_FREE)
    ap.add_argument("--max-swap", type=float, default=MAX_SWAP, help="GB (default %g)" % MAX_SWAP)
    ap.add_argument("--interval", type=float, default=2.0)
    a = ap.parse_args()
    log("watching: kill below %d%% free or above %.1f GB swap (now %d%% free, %.1f GB swap)" % (
        a.min_free, a.max_swap, free_pct(), swap_gb()))
    while True:
        if check(a.min_free, a.max_swap) is not None:
            time.sleep(3)
        time.sleep(a.interval)


if __name__ == "__main__":
    main()
