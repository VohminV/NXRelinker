# NRO Format Notes

Status: header parsing implemented and tested; segment extraction, compression
and asset parsing are **not** implemented.

Primary source: Switchbrew NRO page (`https://switchbrew.org/wiki/NRO`), checked
2026-11-04. The page's latest revision at the time of access was 2025-12-19 (revision 14197).
Implementation: `include/nxrelinker/binary/nro.h`, `src/binary/nro.cpp`.

## Layout

NRO is the executable format for non-ExeFS binaries. It starts with a 0x10-byte
Rocrt structure, followed by the `NRO0` header. The parser therefore verifies the
signature at **offset 0x10**, not offset 0. NRO files may have an optional
homebrew asset section after the declared NRO size.

All integer fields are little-endian.

| Offset | Size | Field | Notes |
| --- | --- | --- | --- |
| 0x00 | 4 | Rocrt ModuleHeaderLocation | 0xC on 17.0.0+, 0x8 before 17.0.0 |
| 0x04 | 0xC | Rocrt reserved/padding | |
| 0x10 | 4 | Signature | `NRO0` |
| 0x14 | 4 | Version | Documented as always 0 for this header |
| 0x18 | 4 | Size | Total NRO file size |
| 0x1C | 4 | Flags | See flag table |
| 0x20 | 4 | TextMemoryOffset | Module-relative memory offset |
| 0x24 | 4 | TextSize | |
| 0x28 | 4 | RoMemoryOffset | |
| 0x2C | 4 | RoSize | |
| 0x30 | 4 | DataMemoryOffset | |
| 0x34 | 4 | DataSize | |
| 0x38 | 4 | BssSize | |
| 0x3C | 4 | AdditionalHeaderOffset | 23.0.0+; optional `NRA0` header |
| 0x40 | 0x20 | ModuleId | Build ID digest |
| 0x60 | 4 | DsoHandleOffset | |
| 0x64 | 4 | Reserved | |
| 0x68 | 4 | EmbeddedOffset | |
| 0x6C | 4 | EmbeddedSize | |
| 0x70 | 4 | DynStrOffset | |
| 0x74 | 4 | DynStrSize | |
| 0x78 | 4 | DynSymOffset | |
| 0x7C | 4 | DynSymSize | |

The NRO header itself is 0x70 bytes relative to its `NRO0` signature at 0x10, so
the minimum parsed span is 0x80 bytes from the start of the file.

## Flags

| Bit | Meaning |
| --- | --- |
| 0 | AlignedHeader |
| 1 | Compress (23.0.0+) |

Unknown flag bits are diagnosed.

## Additional NRO header

When `AdditionalHeaderOffset` is non-zero, the parser checks that a 0x80-byte
region at that offset is inside the file and that it starts with `NRA0`. It does
not yet parse the `CompressedSize`, hash size/offset or hash fields. A missing or
malformed `NRA0` signature is a warning, not a fatal parse error.

## Parser behavior

Fatal diagnostics (no header returned):

- input too short to contain a signature at 0x10;
- missing `NRO0` at offset 0x10;
- input shorter than the 0x80-byte minimum header span.

Recoverable diagnostics (header returned):

- version not zero;
- total size smaller than the header;
- total size larger than the input file (truncated);
- trailing bytes after the declared size (possible asset section);
- unknown flag bits;
- compressed image (reported as unsupported for extraction);
- non-zero reserved field at 0x64;
- additional-header range or `NRA0` signature problems;
- embedded data outside the rodata segment;
- segment memory range beyond the 32-bit module address space;
- overlapping segment memory ranges;
- data plus BSS address-space overflow.

## Important limitation: segment file offsets

The NRO header stores **memory** offsets and sizes for text, ro and data, not
file offsets. The exact file placement rules are not fully verified
(`R-0006`). The inspection report therefore emits `file_offset: null` and
`file_size: null` for NRO segments rather than guessing values. This is
intentional and tested.

## Not implemented

- NRO segment extraction and decompression.
- `NRA0` field parsing.
- Homebrew `ASET` asset parsing.
- NACP/icon/romfs asset handling.
- Relocation or dynamic-symbol processing.
