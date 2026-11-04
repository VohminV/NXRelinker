---
name: arm64-recompilation
description: Use for AArch64 decoding, disassembly, CFG, IR, x86-64 code generation, NZCV, SIMD/FP, atomics, ABI translation and ARM64 differential tests in NXRelinker.
---

# ARM64 Recompilation

## Purpose

Translate first-generation Switch AArch64 code into native x86-64 while
preserving observable program semantics. This is not an interpreter and not an
emulator. The skill covers decoding, CFG, IR, register mapping, flags, memory
semantics, ABI translation and differential testing.

## When to use

- Implementing or reviewing an ARM64 decoder or disassembler.
- Building CFGs, function boundaries or data-flow analysis.
- Defining ARM64 IR or x86-64 lowering.
- Translating NZCV, SP, LR, PC-relative addressing, SIMD/NEON or floating point.
- Handling atomics, exclusive monitors, memory ordering or unaligned accesses.
- Adding differential tests.

## Required sources

- Arm Architecture Reference Manual and instruction documentation:
  `https://developer.arm.com/documentation/`
- A-Profile architecture:
  `https://developer.arm.com/Architectures/A-Profile%20Architecture`
- LLVM MC / disassembler / code generator:
  `https://llvm.org/docs/CodeGenerator.html`,
  `https://llvm.org/docs/ORCv2.html`,
  `https://github.com/llvm/llvm-project`
- Switchbrew GPU page for GPU-specific ARM64 context:
  `https://switchbrew.org/wiki/GPU`
- `docs/research/REFERENCE_SOURCES.md` and `RESEARCH_LOG.md`

Emulator shader/code recompilers are behavioral references only. Do not copy
their architecture or treat them as the primary authority.

## Correctness model

For every supported instruction family, define:

1. Decode: exact mask, match value, fixed and variable fields.
2. Input state: registers, flags, memory, alignment, exception state.
3. Output state: register writes, flag writes, memory effects, PC.
4. Behavioral test vectors: normal, boundary, flag-producing and faulting cases.
5. x86-64 lowering and a differential test comparing observable results.

A successful build or a disassembly match is not proof of semantic correctness.

## Non-negotiable rules

- Never interpret ARM64 and call it recompilation.
- Never emulate the Tegra X1 or embed an emulator as a shortcut.
- Unsupported instructions must trap or produce a diagnosed failure. Never emit
  a silent `nop` or approximate arithmetic.
- Do not hand-wave NZCV, sign extension, 32-bit write zeroing, SP alignment or
  memory ordering.
- Keep Horizon service policy and NVN state out of the recompiler.
- Optimize only after differential tests pass.
- Do not add a public recompiler API without an architect ADR.

## Workflow

1. **Read the current state.** Inspect `include/nxrelinker/arm64/`,
   `src/arm64/`, `include/nxrelinker/recompiler/`, `src/recompiler/` and tests if
   they exist. In Phase 0 they do not; do not create fake empty modules.
2. **Record the encoding.** Cite the ARM manual section and bit fields. If not
   fully verified, mark `INVESTIGATING`.
3. **Write the differential harness first.** Generate inputs and capture
   architectural state without relying on a commercial binary.
4. **Implement decoding** with explicit masks and diagnostics for reserved or
   unsupported encodings.
5. **Build IR** that models flags and memory effects explicitly.
6. **Lower to x86-64** with a documented register mapping and ABI translation.
7. **Test edge cases:** min/max values, carry/borrow, signed overflow, shift
   counts of 0/31/32/63/64, unaligned addresses, exclusive monitor failures,
   NaN and rounding modes.
8. **Run** Release, Debug and ASan/UBSan. Add benchmarks only after correctness.

## Instruction-family checklist

- [ ] ADD/SUB with and without carry, 32/64-bit widths.
- [ ] Logical operations and flag variants.
- [ ] MOV/MOVZ/MOVN/MOVK and wide immediates.
- [ ] ADR/ADRP and PC-relative literal loads.
- [ ] LDR/STR, addressing modes, sign/zero extension, pair loads/stores.
- [ ] B/BL, BR/BLR, RET, compare-and-branch.
- [ ] Conditional select and conditional compare.
- [ ] SIMD/NEON integer and lane operations.
- [ ] Floating point arithmetic, conversions, comparisons and rounding.
- [ ] Atomics and exclusive accesses, including failure and ordering.
- [ ] System instructions used by Switch user code.
- [ ] Exception-related state that the runtime must model.

## Register and ABI notes

- AArch64 has 31 general registers plus SP and ZR; x86-64 register pressure
  requires an explicit mapping and spill strategy.
- Writes to W registers zero the upper 32 bits of the X register.
- NZCV must be materialized or maintained explicitly; x86 flags do not map
  one-to-one for all operations.
- SP must remain 16-byte aligned at public call boundaries.
- LR (X30) and PC-relative semantics must be modeled per instruction, not
  inferred from a function-level convention.
- FP/SIMD register files differ in width and lane layout; conversions need
  explicit tests.
- Memory ordering and exclusives need a runtime contract, not ad-hoc atomics.

## Common mistakes

- Using host flags directly and assuming they match NZCV.
- Forgetting that 32-bit writes clear the upper half of the X register.
- Treating SP as a normal general-purpose register.
- Ignoring the difference between pre-index and post-index addressing.
- Assuming all memory accesses are aligned.
- Using undefined behavior for signed overflow or shifts.
- Falling back to an interpreter for "hard" instructions.
- Claiming SIMD support without lane-level tests.
- Optimizing before the differential suite is green.

## Checks

- [ ] Encoding masks and fields are cited.
- [ ] Every supported encoding has positive and boundary tests.
- [ ] Unsupported encodings fail explicitly.
- [ ] Differential tests compare registers, flags, memory and PC/exception state.
- [ ] No interpreter fallback.
- [ ] ASan/UBSan clean.
- [ ] Performance measurements are recorded only after correctness.

## Completion criteria

- The supported instruction set is documented.
- Differential tests pass for every supported family.
- Unsupported behavior is diagnosed, not approximated.
- The recompiler has no Horizon or graphics policy.
- Public API changes have an ADR.

## Related tests

Future layout:

- `tests/arm64/` — decode and semantics tables.
- `tests/recompiler/` — differential execution and ABI tests.
