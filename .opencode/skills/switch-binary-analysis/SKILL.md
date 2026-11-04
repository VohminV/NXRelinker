---
name: switch-binary-analysis
description: Use when analyzing Nintendo Switch executables (NSO, NRO, ELF64, MOD0, relocations, symbols, imports/exports) or extending NXRelinker binary parsers.
---

# Switch Binary Analysis

## Purpose

Analyze first-generation Nintendo Switch executable formats and turn verified
structure into bounds-checked parsers, diagnostics and tests. This skill covers
NSO, NRO, ELF64 AArch64, MOD0, segments, sections, dynamic tables, relocations,
symbols and imports/exports.

## When to use

- Adding or changing `include/nxrelinker/binary/` or `src/binary/`.
- Investigating a malformed executable or a new NSO/NRO variant.
- Extending `nxinspect` or the JSON report schema.
- Before relocation, symbol or module-linking work.
- When a binary claim needs a source or a synthetic test.

## Current repository boundary

Implemented: signature detection and header parsing for NSO (`NSO0` at 0x0),
NRO (`NRO0` at 0x10) and ELF64. Not implemented: decompression, relocation
application, symbols, imports/exports, disassembly, execution. Do not describe
unimplemented stages as working.

## Required sources

1. `docs/research/REFERENCE_SOURCES.md` — mandatory and verified source registry.
2. `https://switchbrew.org/wiki/NSO` — NSO header, flags, dynamic tables.
3. `https://switchbrew.org/wiki/NRO` — Rocrt/NRO header and additional header.
4. `https://switchbrew.org/wiki/NCA` — PFS0/RomFS context; PFS0/RomFS standalone
   pages were 404 on 2026-11-04.
5. `https://switchbrew.org/wiki/MOD` — MOD0 when module-header work begins.
6. `https://github.com/switchbrew/libnx` — homebrew structures such as NRO/NACP.
7. `https://github.com/switchbrew/switch-tools` — `elf2nso`/`elf2nro` reference.
8. `docs/research/RESEARCH_LOG.md` — known hypotheses and source gaps.

If a source is dead, retain the original URL, record the failure and add a
verified alternative. Never silently replace history.

## Workflow

1. **Read the code first.** Inspect the parser, public header, tests and report
   code that the change touches.
2. **Identify the exact field.** Record offset, width, endianness and semantics
   from a primary source. If uncertain, log `UNVERIFIED` in the research log.
3. **Classify failures.** Fatal: signature, undersized header, unsupported ELF
   class/endianness. Recoverable: out-of-range optional tables, overlaps, unknown
   flags, size mismatches.
4. **Use checked reads.** All accesses go through `has_bytes`, `read_u32_le`,
   `read_u64_le` or equivalent. Use 64-bit range math.
5. **Build synthetic data.** Extend `tests/binary/synthetic_builders.h`; never
   use commercial files.
6. **Test both paths.** Valid parse, fatal invalid input, recoverable
   diagnostics, boundary values.
7. **Keep reports stable.** Add fields to `BinaryInspection` and JSON without
   breaking `schema_version` semantics; update tests and docs.
8. **Document limits.** Update `docs/formats/` and the research log.

## Format-specific guardrails

### NSO

- Header is 0x100 bytes. Do not read fields past it without a size check.
- Segments are text/ro/data with file offset, memory offset, decompressed size
  and a compressed-size field at 0x60/0x64/0x68.
- Compression flags are bits 0/1/2; hash flags are 3/4/5; bits 6 and 7 have
  version-specific meaning.
- The parser uses decompressed size as the file size for uncompressed segments
  and compressed size for compressed segments. The uncompressed-size semantics
  remain `INVESTIGATING` (R-0005).
- Hashes are not verified yet.

### NRO

- `NRO0` is at 0x10 after the 0x10-byte Rocrt structure.
- The header encodes memory offsets/sizes, not file offsets. Do not invent file
  placement.
- Total size at 0x18 may be smaller than the file because a homebrew asset
  section may follow; report trailing data as info.
- `NRA0` is optional and indicated by the field at 0x3C.
- Compressed NRO semantics are `UNVERIFIED` (R-0007).

### ELF64

- Validate class, endianness, machine, header size and table bounds.
- A non-AArch64 machine is a warning so the report remains useful; do not
  silently treat it as Switch code.
- Program/section headers are not parsed yet.

## Common mistakes

- Detecting NRO by `NRO0` at offset 0 instead of 0x10.
- Treating NRO memory offsets as file offsets.
- Assuming every NSO is compressed or has the same optional tables.
- Reading `u32` through an unaligned pointer cast.
- Failing to check `offset + size` with overflow-safe arithmetic.
- Returning a fake success value for a fatal parse.
- Claiming relocation or symbol support because a header field exists.
- Using an emulator repository as the only source for a format field.
- Adding a commercial binary to tests.

## Checks

- [ ] Every field has a cited offset/size/semantics.
- [ ] All reads are bounds-checked and little-endian.
- [ ] Fatal vs recoverable paths are tested.
- [ ] File and memory overlap checks exist where applicable.
- [ ] JSON remains parseable and `schema_version` is updated only deliberately.
- [ ] `docs/formats/` and `RESEARCH_LOG.md` reflect new facts.
- [ ] Release, Debug and ASan/UBSan tests pass.

## Completion criteria

- Parser behavior matches a primary source or a recorded, tested hypothesis.
- Synthetic tests cover valid, truncated, invalid-magic and boundary cases.
- Unsupported features are explicit in code, docs and the task report.
- No regression in existing tests or report output.

## Related tests

- `tests/binary/test_format_detection.cpp`
- `tests/binary/test_nso.cpp`
- `tests/binary/test_nro.cpp`
- `tests/binary/test_elf.cpp`
- `tests/binary/test_inspection.cpp`
