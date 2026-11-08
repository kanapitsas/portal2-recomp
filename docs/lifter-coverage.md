# i386 → C translator coverage (audit of October 25, 2026)

Portal 2 only, before `shaderapivk.so` was added. For Half-Life 2, pass
`--layout build/hl2/prep/layout.json`.

Command (≈ 1 min, 11 cores; the disassembly cache lives in `build/disasm-cache`):

```sh
python3 tools/progress.py --all --compile-forms --layout build/game/prep/layout.json --out build/coverage.json
```

Roots: symbols, exports, `DT_INIT/FINI`, `init_array/fini_array` and code pointers
in data (`data_ptrs`) from **all** prepared modules, followed through direct calls.
Collection mode: every rejected instruction is counted without stopping the function
(`lift.scan`); `lift.lift`, which is used for generation, stays fail-closed.
`--compile-forms` compiles (`clang -fsyntax-only -Werror`) one translated example of each
distinct form (mnemonic + operand shape) found in the game.

## Results

| | before this batch | after |
|---|---|---|
| functions discovered | 115,431 | 115,745 |
| fully translatable | 110,706 (95.9%) | 115,745 (100%) |
| instructions | 9.31 M | 9.79 M (including switch cases) |
| distinct forms compiled | — | 586, all compile |

Blockers fixed, by number of functions affected: `sqrtss` (1,506), "no
instruction boundary" (1,515: cross-module jumps miscounted, and
null bytes that objdump was eliding — fixed with `objdump -z`), `minss`/`maxss`,
`andnps`, `bt/bts/btr/btc`, `shufps`, `unpck*ps`, `movhps/movlps/movlhps/movhlps`,
`movmskps`, `rcpps/rsqrtps/sqrtps`, `fcomi/fucomi(p)`, `fcmovcc`, `fldcw/fnstcw/fnstsw`,
`fi{add,mul,sub,subr,div,divr}`, `frndint`, `fsin/fcos/fptan/fpatan/fyl2x/fprem`,
`fld/fstp tbyte`, MMX (`movq/movd mm`, `padd*/psub*/pmul*/pack*/punpck*/psra*`, `emms`),
SSE2 integer (`pshufd`, `pcmpeq/gt`, `pmuludq`, `packssdw`, `psrldq`, `punpck*`),
8- and 16-bit `mul/imul/div/idiv`, `cbw/cwd`, `sahf/lahf`, `rep ret`, `prefetch*`.
Each form has a semantic test on real machine code (`tests/lifter_programs.s`,
assembled with `clang -target i386-linux-gnu`) in `tests/checks_lifter.c`.

## Indirect jumps

| category (occurrences in discovered functions) | count |
|---|---|
| switch tables resolved statically | 5,441 |
| remaining `jmp reg` (virtual tail calls) | 6,169 |
| remaining `jmp [mem]` (vtables, pointers) | 2,458 |
| PLT stubs | 1,377 |

Across all recognizable table shapes, 1,632 are resolved and 1 remains (an
array of function pointers in studiorender, not a switch). A resolved table
becomes a C `switch` of local `goto`s, but **the target computed by the guest code
is checked**: any other value goes through a checked `rt_dispatch` (which traps if it
was not recompiled). An imprecise table can therefore only add dead labels.

## Known limitations

- Static: indirect targets outside `data_ptrs` (constructed vtables, callbacks)
  are not discovered; "translatable" means neither executed nor correct.
- x87 runs in double precision (not 80-bit); `fld/fstp tbyte` convert. `rcpps/rsqrtps`
  are exact (within the ISA bound of 1.5·2⁻¹², not bit-for-bit with Intel). ARM default NaN.
- MMX: `mm` registers are separate from the x87 stack (no aliasing); TOP/tags are updated.
- C++ exceptions: only libstdc++ error paths (bad_alloc, bad_cast,
  length_error, the verbose terminate handler) throw; there is no `catch` in the game's
  code. A real `throw` that was tested (libsteam_api_linux `std::__throw_bad_cast`) unwinds the
  guest stack through translated `dl_iterate_phdr`/`.eh_frame`, finds no handler
  and ends up in `abort`, just as on Linux. Landing pads are not resume
  points: if a handler did exist, the transfer would unwind up to a native
  boundary and trap explicitly.
