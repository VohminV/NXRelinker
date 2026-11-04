---
name: nso-nro-formats
description: Use when working with NSO or NRO header fields, Rocrt, NRA0, NSO compression flags, module IDs, segment ranges or synthetic NSO/NRO test data.
---

# NSO and NRO Formats

## Purpose

Provide a precise, source-checked workflow for NSO and NRO structure work. This
skill is intentionally narrower than `switch-binary-analysis`: it focuses on the
two executable containers the repository currently parses.

## When to use

- Editing `nso.h`, `nro.h`, `nso.cpp`, `nro.cpp` or their tests.
- Investigating a header offset, flag, segment range or module ID.
- Adding synthetic NSO/NRO fixtures.
- Reviewing an `nxinspect` report for a Switch executable.
- Before implementing decompression or NRO assets.

## Mandatory sources

- `https://switchbrew.org/wiki/NSO` (checked 2026-11-04)
- `https://switchbrew.org/wiki/NRO` (checked 2026-11-04)
- `https://switchbrew.org/wiki/NCA` for PFS0/RomFS context when needed
- `https://github.com/switchbrew/libnx` for fixed-width structures
- `https://github.com/switchbrew/switch-tools` for `elf2nso`/`elf2nro`
- `docs/formats/NSO.md`, `docs/formats/NRO.md`
- `docs/research/RESEARCH_LOG.md` entries R-0001, R-0002, R-0005, R-0006, R-0007

The standalone `File_formats`, `PFS0` and `RomFs` wiki pages returned 404 on the
check date. Do not pretend otherwise.

## NSO verification checklist

- [ ] `NSO0` at 0x0; header length at least 0x100.
- [ ] Version at 0x4, reserved at 0x8, flags at 0xC.
- [ ] Three segments at 0x10/0x20/0x30, each: file offset, memory offset,
      decompressed size.
- [ ] Module name offset/size at 0x1C/0x2C.
- [ ] BSS at 0x3C, module ID at 0x40, compressed sizes at 0x60/0x64/0x68.
- [ ] Embedded/dynstr/dynsym offsets at 0x88-0x9C (relative to rodata).
- [ ] Hash fields at 0xA0/0xC0/0xE0 are read but not verified.
- [ ] Compression bits 0/1/2 and hash bits 3/4/5 are interpreted separately.
- [ ] Unknown bits 8+ are diagnosed, not assumed reserved.
- [ ] Effective file size is compressed size when the flag is set, decompressed
      size otherwise; a mismatch is diagnosed.
- [ ] File and memory overlap checks are 64-bit safe.
- [ ] Data + BSS address-space overflow is diagnosed.

## NRO verification checklist

- [ ] Rocrt is 0x10 bytes; `NRO0` is at 0x10.
- [ ] Version 0x14, total size 0x18, flags 0x1C.
- [ ] Text/ro/data memory offsets and sizes at 0x20-0x34.
- [ ] BSS at 0x38, additional-header offset at 0x3C, module ID at 0x40.
- [ ] DSO/embedded/dynstr/dynsym fields at 0x60-0x7C.
- [ ] Minimum parsed span is 0x80 bytes.
- [ ] Total size smaller than header or larger than file is an error.
- [ ] Trailing bytes after total size are info (possible assets).
- [ ] `NRA0` presence is checked when the additional-header offset is non-zero.
- [ ] Segment file offsets are reported as null; they are not encoded in the
      header.
- [ ] Compressed flag is diagnosed as unsupported for extraction.
- [ ] Overlap and address-space checks are 64-bit safe.

## Synthetic fixture rules

1. Allocate a zero-filled buffer large enough for the header plus referenced
   data.
2. Write magic, fields and module ID explicitly with little-endian helpers.
3. Keep segment ranges non-overlapping in the valid fixture.
4. Derive invalid cases by mutating one field, not by reusing a commercial file.
5. Assert diagnostic codes, not human-readable messages.
6. Keep NRO segment `file_offset`/`file_size` unset in report tests.

## Common mistakes

- Writing `NRO0` at offset 0 in a synthetic fixture.
- Forgetting the Rocrt prefix when computing NRO header offsets.
- Using 0x2C for an NRO segment size instead of BSS/additional fields.
- Treating the NSO module-name size field as a segment field.
- Using the compressed-size field for an uncompressed segment without a
  diagnostic.
- Assuming `NroHeader.size` equals the file size even when assets follow.
- Reporting an NRO file offset that the header does not contain.
- Testing only the happy path.

## Workflow

1. Read `docs/formats/NSO.md` or `docs/formats/NRO.md`.
2. Open the parser and public header side by side.
3. Check the relevant research entry for known ambiguities.
4. Make the smallest parser/report change.
5. Extend the synthetic builder if needed.
6. Add or update tests for valid, truncated, invalid-magic and structural cases.
7. Run Release, Debug and sanitizer tests.
8. Update the format doc and research log if a fact changes.

## Completion criteria

- Field layout and semantics are source-backed.
- Every new field has a test.
- Fatal and recoverable diagnostics are distinguished.
- The report remains valid JSON and the text report is readable.
- Unsupported compression, hashing or extraction paths are documented.

## Related tests

- `tests/binary/test_nso.cpp`
- `tests/binary/test_nro.cpp`
- `tests/binary/test_format_detection.cpp`
- `tests/binary/test_inspection.cpp`
- `examples/synthetic/make_synthetic.py`
