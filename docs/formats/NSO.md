# NSO Format Notes

Status: header parsing implemented and tested; decompression and relocation
processing are **not** implemented.

Primary source: Switchbrew NSO page (`https://switchbrew.org/wiki/NSO`), checked
2026-11-04. The page's latest revision at the time of access was 2025-12-19 (revision 14193).
Implementation: `include/nxrelinker/binary/nso.h`, `src/binary/nso.cpp`.

## Layout

NSO is the main executable format. The header is 0x100 bytes and describes three
segments: text, rodata and data. Their file positions come from the
file-offset fields, not from a fixed concatenation order. Compressed segments
use LZ4 in the legacy encoding and a zstd-based scheme (ZBIC) on newer system
versions.

All integer fields are little-endian.

| Offset | Size | Field | Notes |
| --- | --- | --- | --- |
| 0x00 | 4 | Signature | `NSO0` |
| 0x04 | 4 | Version | Documented as always 0 |
| 0x08 | 4 | Reserved | Parser warns if non-zero |
| 0x0C | 4 | Flags | See flag table |
| 0x10 | 4 | TextFileOffset | File offset of `.text` |
| 0x14 | 4 | TextMemoryOffset | Module-relative memory offset |
| 0x18 | 4 | TextSize | Decompressed size |
| 0x1C | 4 | ModuleNameOffset | Documented as the header size (0x100) |
| 0x20 | 4 | RoFileOffset | File offset of `.rodata` |
| 0x24 | 4 | RoMemoryOffset | Module-relative memory offset |
| 0x28 | 4 | RoSize | Decompressed size |
| 0x2C | 4 | ModuleNameSize | String length in bytes |
| 0x30 | 4 | DataFileOffset | File offset of `.data` |
| 0x34 | 4 | DataMemoryOffset | Module-relative memory offset |
| 0x38 | 4 | DataSize | Decompressed size |
| 0x3C | 4 | BssSize | Zero-initialized bytes after `.data` |
| 0x40 | 0x20 | ModuleId | Build ID digest, up to 32 bytes |
| 0x60 | 4 | TextFileSize | Compressed `.text` size |
| 0x64 | 4 | RoFileSize | Compressed `.rodata` size |
| 0x68 | 4 | DataFileSize | Compressed `.data` size |
| 0x6C | 0x1C | Reserved | |
| 0x88 | 4 | EmbeddedOffset | Relative to `.rodata` |
| 0x8C | 4 | EmbeddedSize | Relative to `.rodata` |
| 0x90 | 4 | DynStrOffset | Relative to `.rodata` |
| 0x94 | 4 | DynStrSize | |
| 0x98 | 4 | DynSymOffset | Relative to `.rodata` |
| 0x9C | 4 | DynSymSize | |
| 0xA0 | 0x20 | TextHash | SHA-256 of decompressed `.text` |
| 0xC0 | 0x20 | RoHash | SHA-256 of decompressed `.rodata` |
| 0xE0 | 0x20 | DataHash | SHA-256 of decompressed `.data` |

## Flags

| Bit | Meaning |
| --- | --- |
| 0 | Text is compressed |
| 1 | Ro is compressed |
| 2 | Data is compressed |
| 3 | Text hash must be checked when loading |
| 4 | Ro hash must be checked when loading |
| 5 | Data hash must be checked when loading |
| 6 | Execute-only text memory (20.0.0+) |
| 7 | ZBIC compression instead of LZ4 (22.0.0+) |

Bits 8 and above are currently diagnosed as unknown; they are not assumed to be
reserved zero.

## Parser behavior

Fatal diagnostics (no header returned):

- input shorter than 4 bytes;
- missing `NSO0` at offset 0;
- input shorter than the 0x100-byte header.

Recoverable diagnostics (header returned):

- version not zero;
- reserved field not zero;
- unknown flag bits;
- module name range outside the file;
- compressed segment with zero compressed size;
- uncompressed segment whose compressed-size field is non-zero and disagrees with
  the decompressed size;
- segment file range outside the input;
- overlapping segment file or memory ranges;
- data plus BSS extending beyond the 32-bit module address space;
- embedded/dynamic-table ranges outside `.rodata`.

For uncompressed segments the physical file size is the decompressed size. For
compressed segments the physical file size is the corresponding compressed-size
field. This distinction is recorded from the documented field semantics and is
covered by tests.

## Not implemented

- LZ4 or ZBIC decompression.
- SHA-256 verification.
- Relocation application, dynamic symbol/string parsing.
- Execution or mapping of segments.
- Validation of the module name contents.
- A claim that every NSO uses the same segment set or compression flags.

The parser reports what the header declares. It does not assume that all NSOs are
compressed or that optional tables are present.
