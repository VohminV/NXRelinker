# ELF64 Format Notes

Status: ELF64 AArch64 header validation implemented and tested; program-header
and section-header entry decoding, symbols, relocations and execution are **not**
implemented.

Primary sources: System V gABI ELF header (`https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html`)
and Linux `elf(5)` (`https://man7.org/linux/man-pages/man5/elf.5.html`), checked
2026-11-04. Implementation: `include/nxrelinker/binary/elf.h`,
`src/binary/elf.cpp`.

## Identification

The parser accepts only:

- magic `\x7FELF` at offset `0x0`;
- `EI_CLASS = ELFCLASS64` (`2`);
- `EI_DATA = ELFDATA2LSB` (`1`, little-endian);
- AArch64 machine `EM_AARCH64 = 183` for the normal Switch target.

An input shorter than `0x40` bytes is rejected before any field is used.

## Header layout

| Offset | Size | Field | Parser behavior |
| --- | --- | --- | --- |
| 0x00 | 4 | `e_ident[EI_MAG0..3]` | Fatal if not `\x7FELF` |
| 0x04 | 1 | `EI_CLASS` | Fatal unless `ELFCLASS64` |
| 0x05 | 1 | `EI_DATA` | Fatal unless little-endian |
| 0x06 | 1 | `EI_VERSION` | Warning if not `1` |
| 0x07 | 1 | `EI_OSABI` | Reported, not interpreted |
| 0x08 | 1 | `EI_ABIVERSION` | Reported, not interpreted |
| 0x09 | 7 | padding | Ignored |
| 0x10 | 2 | `e_type` | Warning unless `ET_EXEC` or `ET_DYN` |
| 0x12 | 2 | `e_machine` | Warning unless `183` |
| 0x14 | 4 | `e_version` | Warning if not `1` |
| 0x18 | 8 | `e_entry` | Reported |
| 0x20 | 8 | `e_phoff` | Range-checked with `e_phnum * e_phentsize` |
| 0x28 | 8 | `e_shoff` | Range-checked with `e_shnum * e_shentsize` |
| 0x30 | 4 | `e_flags` | Reported |
| 0x34 | 2 | `e_ehsize` | Error if not `0x40` |
| 0x36 | 2 | `e_phentsize` | Error if program headers exist and size is not `56` |
| 0x38 | 2 | `e_phnum` | Warning if zero |
| 0x3A | 2 | `e_shentsize` | Error if section headers exist and size is not `64` |
| 0x3C | 2 | `e_shnum` | Reported |
| 0x3E | 2 | `e_shstrndx` | Warning if outside the section table |

## Parser behavior

Fatal diagnostics (no header returned):

- input shorter than 4 bytes;
- missing ELF signature at offset 0;
- input shorter than the 0x40-byte ELF64 header;
- unsupported ELF class;
- unsupported endianness.

Recoverable diagnostics (header returned):

- invalid `e_ehsize`;
- `EI_VERSION` or `e_version` not equal to 1;
- non-AArch64 machine;
- type other than `ET_EXEC`/`ET_DYN`;
- invalid program/section header entry size;
- program/section header table outside the input;
- no program headers;
- section name string-table index outside the section table.

For thunks and homebrew, a non-AArch64 or non-executable ELF can still be
inspected; diagnostics explain why it is not a normal Switch target.

## Deliberate Phase 0 limits

- Program-header and section-header entries are not decoded.
- `PN_XNUM` and extended section numbering are not resolved.
- Symbols, string tables, relocations and dynamic entries are not parsed.
- No loading, mapping or execution is performed.
- No assumption is made that any arbitrary ELF is a Switch executable.

## Open questions

- Which non-AArch64 ELF variants, if any, should NXRelinker accept for tooling?
- How should extended numbering be surfaced before Phase 1 table decoding?
