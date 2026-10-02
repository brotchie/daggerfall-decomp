#!/usr/bin/env python3
"""Explain a function's stack layout with the -od slot rule and infer which variables were
2-byte (short) in the source.

Rule (docs/progress.md), top of the frame down: 2-byte parameters last to first, 2-byte
locals, the return variable, locals, parameters last to first, nested-block locals. Locals'
relative order is free (the lifter declares them in slot order), parameters' is not.

plan(params_top_down, slots) tries every subset of parameters as 2-byte and returns
(small_params, small_locals, nested_locals) or None.
"""
import itertools


def plan(tokens, nparams):
    """tokens: the frame top down, each ('P', i) for parameter i (1-based), ('R',) or
    ('L', off). Returns (set of small params, set of small local offsets, set of nested local
    offsets) or None if no assignment fits."""
    for k in range(nparams + 1):
        for small in itertools.combinations(range(1, nparams + 1), k):
            res = fit(tokens, set(small), nparams)
            if res is not None:
                return res
    return None


def fit(tokens, small, nparams):
    big = [i for i in range(nparams, 0, -1) if i not in small]
    sm = [i for i in range(nparams, 0, -1) if i in small]
    # expected parameter order top down: small ones (last to first) ... big ones (last to first)
    seq = [t for t in tokens]
    pos = 0
    small_l, nested_l = set(), set()
    # 1. small params
    for i in sm:
        if pos >= len(seq) or seq[pos] != ("P", i):
            return None
        pos += 1
    # 2. small locals, until R or a big param or the end
    has_r = any(t == ("R",) for t in seq)
    if has_r:
        while pos < len(seq) and seq[pos][0] == "L":
            small_l.add(seq[pos][1])
            pos += 1
        if pos >= len(seq) or seq[pos] != ("R",):
            return None
        pos += 1
    # 3. locals
    while pos < len(seq) and seq[pos][0] == "L":
        pos += 1
    # 4. big params
    for i in big:
        if pos >= len(seq) or seq[pos] != ("P", i):
            return None
        pos += 1
    # 5. nested locals
    while pos < len(seq) and seq[pos][0] == "L":
        nested_l.add(seq[pos][1])
        pos += 1
    if pos != len(seq):
        return None
    return set(small), small_l, nested_l
