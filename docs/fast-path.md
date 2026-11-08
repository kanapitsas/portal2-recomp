# Fast paths in generated code

Excluding host shims, three costs dominated the translated code: software
memory checks (10 to 14 instructions per access), routing every call through
`rt_call` → `rt_dispatch` → `rt_lookup`, and reloading guest registers after
every write to guest memory.

## What changed

- **Registers**: generated functions take `CPU *restrict c` and copy the
  memory base into a local `mem_` (`RT_MEM` macro, see `rt.h`). Runtime
  functions that take the CPU are declared `noescape`.
  Invariants: the CPU lives outside guest memory, and no runtime function keeps
  the pointer after it returns (`shim_cpu` and `once_cpu` are restored).
- **Calls**: a `call`/`jmp` to a fixed, translated address becomes
  `rt_call_fn`/`rt_jump_fn` (a direct C call). `link_direct` (lift.py) falls
  back to `rt_call`/`rt_dispatch` for any untranslated target, and to
  `rt_call_import` (shim route already resolved) for imports. Indirect calls
  and jumps go through `rt_call_any`/`rt_jump_any` (`rt_lookup` inlined). The
  protocol for non-local transfers is unchanged: the caller compares `c->eip`
  with its return address. Every fast path yields the guest lock via `rt_poll`
  (an inline test of `rt_guest_waiting`).
- **Memory**: `RT_FAST_MEMORY` removes the per-access software check.
  Protection then comes from the 16 KiB host pages (the union of the
  permissions of the four 4 KiB Linux pages, `vm.c`). A 64 KiB guard page
  after the 4 GB absorbs accesses that run past the end.

## Build options (`tools/build_game.py`)

| Option | Effect | Use |
|---|---|---|
| (default) | memory checks at 4 KiB granularity, direct calls | development |
| `--release` | `--opt 2 --fast-memory` | what `setup.sh` builds, to play |
| `--fast-memory` | `-DRT_FAST_MEMORY` | benchmarking, once the game is validated |
| `--traced-calls` (alias `--debug-traces`) | `-DRT_TRACED_CALLS`: every call goes through `rt_call` | full guest stack in faults, `P2_WATCH_CALL` |

`make test` stays in checked mode: `tests/vm_checks.h` verifies the 4 KiB
granularity and fails with `RT_FAST_MEMORY`, as expected.

## Measuring

```sh
python3 tools/bench.py --runs 5 --config base= --config fast=-DRT_FAST_MEMORY
```

The benchmark (`tests/bench.c`) runs real functions from `libtier0` and
`libvstdlib`, generated the same way as the game (`generate_sharded`): binary
codec, UTF-8 ↔ UTF-32, and the `KeyValuesSystem` symbol table. Checksums
must match across configurations. `P2_PROFILE=file` (the built-in profiler)
also works on the benchmark; pass enough iterations (`… 2400`) to exceed the
5 s write interval.

Measurements from October 25, 2026 (M3 Pro, `-O1`, median of 5):

| | codec | unicode | keyvals |
|---|---|---|---|
| baseline (3a8440a) | 36.0 ms | 87.1 ms | 124.3 ms |
| branch, checked memory | 21.8 ms | 89.2 ms | 113.6 ms |
| branch, `--fast-memory` | 22.8 ms | 88.7 ms | 103.7 ms |

`unicode` runs almost entirely inside the host's iconv, and `keyvals` largely
inside host `strcmp`/`strcasecmp`, so this benchmark is a poor measure of the
generated code on its own. The full game compiled to a 354 MB executable
(down from 703 MB) at the time; 359 MB once `shaderapivk.so` was added.

## Known limitations

- Without `--traced-calls`, `frames[]` (the guest stack used for diagnostics)
  no longer sees direct calls or resolved indirect calls, and `P2_WATCH_CALL`
  only sees calls that still go through `rt_call`.
- The fast paths assume guest execution is serialized by the lock
  (`scheduler.c`); nothing here makes the generated code safe to run in
  parallel.
