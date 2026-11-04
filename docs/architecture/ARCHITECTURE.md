# Architecture

## Goal

Convert first-generation Nintendo Switch executables into native PC executables
through analysis, relinking and ARM64-to-x86-64 recompilation, with Horizon OS
and NVN compatibility layers. The result must not depend on an emulator.

## Target pipeline

```text
NSO / NRO / ELF64 AArch64
        |
        v
binary analysis  (formats, segments, MOD0, dynamic tables, relocations)
        |
        v
linker           (module graph, symbols, imports/exports, address remapping)
        |
        v
ARM64 analysis   (decode, CFG, function boundaries, IR, data flow)
        |
        v
x86-64 recompiler (semantics, ABI, SIMD/FP, memory ordering)
        |
        v
native image     (Windows/Linux executable or shared module)
        |
        v
Horizon runtime + NVN compatibility layer + Vulkan
```

Static relinking and native recompilation are independent mechanisms:

1. **Static relinking** restores module structure, resolves relocations and
   symbols, and produces a linked image or module graph.
2. **Native recompilation** translates ARM64 semantics into x86-64 code. It is
   not an ARM64 interpreter.

## Subsystem boundaries

| Subsystem | Owns | Must not own |
| --- | --- | --- |
| `core` | Byte spans, checked arithmetic, parse-result plumbing, hex helpers | Format-specific policy |
| `diagnostics` | Severity values, diagnostic bag, JSON serialization | Binary formats or runtime policy |
| `binary` | NSO/NRO/ELF parsing, structural validation, format detection, inspection reports | Execution, Horizon services, Vulkan |
| `linker` (future) | Module loading, relocations, symbols, dependency graph, address remapping | Recompiler logic |
| `arm64` (future) | Decode, disassembly, CFG, function boundaries, ABI analysis | Horizon service policy, GPU state |
| `recompiler` (future) | ARM64 IR, x86-64 emission, register mapping, flag semantics | Runtime service policy |
| `runtime` / `horizon` (future) | Processes, threads, handles, IPC, memory, services | Executable-format parsing |
| `graphics` / `shaders` (future) | NVN state, command translation, GPU memory, shader IR, SPIR-V | Horizon service policy |
| `assets` (future) | BNTX/BFRES/SARC/AAMP/RomFS/PFS0 parsing and conversion | Recompiler or runtime policy |

Dependencies point downward/leftward in the pipeline. `diagnostics` does not
depend on `binary`. `binary` does not depend on future runtime or graphics
subsystems.

## Current implementation

The Phase 0 library `nxrelinker` is static and contains `core`, `diagnostics`
and `binary`. Sources are split by subsystem even while they share one library
target; future phases can split targets when dependency direction requires it.

Public headers live under `include/nxrelinker/<subsystem>/`. The namespace mirrors
the subsystem:

```cpp
nxrelinker::binary::parse_nso(...)
nxrelinker::diagnostics::DiagnosticBag
nxrelinker::binary::inspect_binary(...)
```

`ParseResult<T>` carries an optional value plus a diagnostic bag. Fatal structural
errors (wrong signature, undersized header, unsupported ELF class) return no
value. Recoverable structural problems (segment outside file, overlaps, unknown
flags) return a parsed header plus diagnostics so callers can produce a detailed
report.

## Parser design rules

- All reads go through checked helpers; no unaligned pointer casts.
- Offsets and sizes are combined in 64-bit arithmetic.
- Fields are read explicitly in little-endian order.
- Structural checks are separated from report formatting.
- No decompression, relocation application or execution happens in Phase 0.
- Unknown flags and reserved fields are diagnosed, not silently ignored.
- Format detection is signature-based at documented offsets, never extension-based.
- Tests use synthetic builders; no commercial binaries are required.

## Data flow for `nxinspect`

```text
argv -> file bytes -> detect_format
                         |
          +--------------+--------------+
          |              |              |
       parse_nso      parse_nro      parse_elf64
          |              |              |
          +--------------+--------------+
                         |
                  BinaryInspection
                         |
              +----------+----------+
              |                     |
         format_text           format_json
```

`BinaryInspection` is a reporting model, not a loader object. Future loading
subsystems will consume parser results through dedicated APIs rather than
reusing the inspection model.

## Repository structure policies

- Create directories only when implementation lands. Empty placeholder trees are
  not committed.
- Keep CLI code in `tools/`; CLIs are thin wrappers over library APIs.
- Tests live under `tests/` and mirror subsystem boundaries.
- Research artifacts live under `docs/research/`, never in source comments as
  unverified claims.
- Public API changes require an ADR.

## Future boundaries and open questions

- Whether `binary` should expose a generic image model or format-specific types
  remains open until relocation and module loading work starts.
- The linker/recompiler interface will be defined with an IR and relocation
  contract before implementation. Do not build a shortcut interpreter.
- Horizon IPC serialization must be specified independently of any emulator
  source before runtime implementation.
- NVN command and shader translation require primary documentation or
  reproducible tests; assumptions must be logged as `UNVERIFIED`.
