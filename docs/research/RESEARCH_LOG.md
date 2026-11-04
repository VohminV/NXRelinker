# Research Log

All findings use the template below. Status values:

- `UNVERIFIED`
- `INVESTIGATING`
- `VERIFIED`
- `IMPLEMENTED`
- `TESTED`
- `REJECTED`

A hypothesis is not a fact. Implementation claims require a test or a primary
source cross-check.

---

## R-0001 — NSO header layout

**Research ID:** R-0001
**Date:** 2026-11-04
**Subsystem:** binary / executable formats
**Source:** Switchbrew NSO page
**Reference URL:** https://switchbrew.org/wiki/NSO
**Technical question:** What are the exact NSO header offsets, sizes, flag bits
and segment fields for the implemented parser?

**Observed behavior:** The page documents a 0x100-byte header with `NSO0` at
0x0, version at 0x4, flags at 0xC, three 12-byte segment descriptors starting at
0x10/0x20/0x30, module-name offset/size at 0x1C/0x2C, BSS at 0x3C, module ID at
0x40, compressed sizes at 0x60/0x64/0x68, optional tables at 0x88-0x9C and three
SHA-256 hashes at 0xA0/0xC0/0xE0. Compression and hash-verification flags occupy
bits 0-7.

**Verified facts:** The field offsets above were read directly from the page on
2026-11-04. The page's latest revision at the time of access was 2025-12-19 (revision 14193). `NSO0` is at file offset 0.

**Hypothesis:** Uncompressed NSO segments have a physical file size equal to the
decompressed size and may leave the compressed-size field zero. This is the
parser's current assumption when choosing the file range; it still needs a
primary-source or multi-sample confirmation.

**Implementation implications:** `src/binary/nso.cpp` parses the documented
fields, validates file/memory ranges and reports unknown flags. It does not
decompress or hash data. The compressed-size ambiguity is diagnosed when the
field disagrees with the decompressed size.

**Test required:** Synthetic tests cover valid, truncated, bad-magic,
out-of-range, overlap, unknown-flag, compressed-zero-size and BSS-overflow cases.
Real-world sample validation is still required for the uncompressed-size
assumption.

**Status:** TESTED (header layout); INVESTIGATING (uncompressed compressed-size
semantics).

---

## R-0002 — NRO/Rocrt header layout

**Research ID:** R-0002
**Date:** 2026-11-04
**Subsystem:** binary / executable formats
**Source:** Switchbrew NRO page; libnx `nro.h`
**Reference URL:** https://switchbrew.org/wiki/NRO
**Technical question:** Where is the NRO signature and what fields does the
Rocrt-prefixed header contain?

**Observed behavior:** The NRO starts with a 0x10-byte Rocrt structure. `NRO0` is
at offset 0x10. The header stores total size at 0x18, flags at 0x1C, three
(memory offset, size) segment pairs starting at 0x20, BSS at 0x38, optional
additional-header offset at 0x3C, module ID at 0x40 and dynamic-table offsets at
0x68-0x7C. The first Rocrt word is the module-header location (0xC on 17.0.0+,
0x8 before). libnx's `NroHeader` is the structure beginning at the `NRO0`
signature; its first field is the magic, which confirms the offset-0x10 start.

**Verified facts:** `NRO0` at 0x10, minimum parsed header span 0x80 bytes,
segment fields are memory offsets/sizes, `NRA0` additional header is optional and
`ASET` assets can follow the declared size.

**Hypothesis:** None required for header parsing. Segment file offsets are not
encoded in the header and must not be guessed.

**Implementation implications:** `src/binary/nro.cpp` uses absolute offsets,
checks total size/trailing assets, additional-header signature, memory overlap
and reserved fields. Inspection JSON emits `file_offset: null` and
`file_size: null` for NRO segments. Compression is reported but not supported.

**Test required:** Synthetic valid, wrong-offset magic, truncated, size-beyond-
file, trailing-assets, compressed-flag, overlap and additional-header tests.

**Status:** TESTED (header layout); INVESTIGATING (compressed NRO segment
semantics and exact file placement rules).

---

## R-0003 — ELF64 AArch64 header validation

**Research ID:** R-0003
**Date:** 2026-11-04
**Subsystem:** binary / executable formats
**Source:** System V gABI ELF header; Linux `elf(5)`; Switchbrew/libnx build outputs
**Reference URL:** https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html ; https://man7.org/linux/man-pages/man5/elf.5.html
**Technical question:** Which ELF header checks are safe and useful before
segment/section parsing is implemented?

**Observed behavior:** ELF64 uses a 0x40-byte header, little-endian for Switch
AArch64, `e_machine = 183` (`EM_AARCH64`), 56-byte program-header entries and
64-byte section-header entries. The parser checks class, endianness,
`EI_VERSION`, `e_version`, machine, table bounds and entry sizes.

**Verified facts:** The ELF magic and field offsets are stable; `EI_VERSION` and
`e_version` are both 1; `e_machine` 183 is AArch64. A non-AArch64 machine is
diagnosed as a warning rather than a fatal error so the report can still
describe the file.

**Hypothesis:** None.

**Implementation implications:** `src/binary/elf.cpp` validates the header and
tables, but does not yet parse segments or sections. Future `MOD0` and dynamic
table work must extend, not replace, this boundary.

**Test required:** Synthetic valid, ELF32, big-endian, non-AArch64, bad table
bounds and truncated tests are implemented.

**Status:** TESTED (header validation); section/program-header parsing remains
unimplemented.

---

## R-0004 — Source availability audit for the mandatory baseline

**Research ID:** R-0004
**Date:** 2026-11-04
**Subsystem:** research infrastructure
**Source:** Automated HTTP checks of every URL in the project prompt
**Reference URL:** See `docs/research/REFERENCE_SOURCES.md`
**Technical question:** Which mandatory sources are reachable, restricted or
gone, and what alternatives are verified?

**Observed behavior:** Most GitHub and documentation URLs returned 200. Several
Switchbrew format pages (`File_formats`, `PFS0`, `RomFs`, `BNTX`, `BFRES`,
`AAMP`, `SARC`) returned 404. The canonical LibHac repository returned 404.
Several historical tool repositories (`NXGameInfo`, `KillzXGaming/SarcLib`,
`Nintendo.NX.SARC`, `roblabla/ida-switch-loader`, `Ghidra-Horizon-Kernel`,
`UAssetAPI/UAssetAPI`, `godot-nvn-shader-converter`) returned 404. Ryujinx's
GitHub organization paths returned 404; its official Forgejo instance was
verified at `https://git.ryujinx.app/ryujinx/ryujinx` and
`https://git.ryujinx.app/ryujinx/ShaderTools`. Discord returned a TLS handshake
timeout; GBAtemp, Reddit and Nexus blocked automated access (403/redirect).

**Verified facts:** The status table in `REFERENCE_SOURCES.md` records the HTTP
results and final URLs. Original URLs were retained. Verified alternatives were
recorded for Ryujinx, SarcLib, ida-switch-loader and UAssetAPI.

**Hypothesis:** Some 404 pages may have been removed for legal reasons rather
than renamed; do not infer the removed content.

**Implementation implications:** NSO/NRO/ELF header work relies on the still
available Switchbrew NSO/NRO/NCA pages, libnx and switch-tools. Asset-format
implementation is blocked on independent source verification or synthetic
reverse-engineering evidence.

**Test required:** Re-run link checks before each phase gate and append a new
research entry when status changes.

**Status:** VERIFIED (availability audit).

---

## R-0005 — NSO uncompressed segment file-size semantics

**Research ID:** R-0005
**Date:** 2026-11-04
**Subsystem:** binary / NSO
**Source:** Switchbrew NSO page
**Reference URL:** https://switchbrew.org/wiki/NSO
**Technical question:** When an NSO segment is uncompressed, is the
`TextFileSize`/`RoFileSize`/`DataFileSize` field authoritative, zero, or equal to
the decompressed size?

**Observed behavior:** The wiki labels these fields as compressed sizes. The
parser must choose a physical file range. Current behavior uses the decompressed
size when the compression flag is clear and warns if a non-zero compressed-size
field disagrees.

**Verified facts:** The field at 0x60/0x64/0x68 is documented as the compressed
size. The compression flag is authoritative for whether a segment is compressed.

**Hypothesis:** Official NSOs are normally compressed; uncompressed synthetic
cases may leave these fields zero. The parser's warning is conservative and
non-fatal.

**Implementation implications:** Do not hard-fail on a non-zero mismatch until a
primary source or a lawfully obtained sample confirms the semantics.

**Test required:** A real-world uncompressed NSO sample or a verified loader
implementation is needed. Until then keep the hypothesis status.

**Status:** INVESTIGATING.

---

## R-0006 — NRO segment file placement

**Research ID:** R-0006
**Date:** 2026-11-04
**Subsystem:** binary / NRO
**Source:** Switchbrew NRO page
**Reference URL:** https://switchbrew.org/wiki/NRO
**Technical question:** Does the NRO header encode file offsets for text, ro and
data segments?

**Observed behavior:** The documented fields are memory offset and size only.
The NRO header starts after a 0x10-byte Rocrt structure, and the file contains
the segment data after the header. The header does not contain explicit file
offset fields for the three segments.

**Verified facts:** Memory offsets/sizes are encoded. Absolute file offsets are
not present in the documented header. The inspection report intentionally emits
null file offsets for NRO segments.

**Hypothesis:** A loader derives file placement from the NRO layout and header
size, possibly with alignment rules. This must be confirmed before implementing
extraction.

**Implementation implications:** Do not compute segment file offsets from memory
offsets. Add a verified extraction design in Phase 1.

**Test required:** A synthetic NRO extraction fixture and a primary-source or
reference-loader cross-check.

**Status:** INVESTIGATING (exact placement); VERIFIED (header lacks file offsets).

---

## R-0007 — Compressed NRO semantics

**Research ID:** R-0007
**Date:** 2026-11-04
**Subsystem:** binary / NRO
**Source:** Switchbrew NRO page
**Reference URL:** https://switchbrew.org/wiki/NRO
**Technical question:** What changes in a compressed NRO (flag bit 1) and how are
segment sizes interpreted?

**Observed behavior:** The flag is documented as `Compress` for 23.0.0+. The
associated `NRA0` additional header contains `CompressedSize`, hash size and hash
offset. The exact compressed stream format and segment-size semantics are not yet
verified.

**Verified facts:** Compressed NROs exist and the parser warns that extraction is
unsupported.

**Hypothesis:** The `NRA0` header describes a compressed image alongside the
uncompressed metadata. Do not implement decompression until the stream format is
verified.

**Implementation implications:** Parsing remains header-only for compressed NROs.

**Test required:** Obtain a primary source or produce a synthetic fixture with a
documented compressor only after verification.

**Status:** UNVERIFIED.

---

## R-0008 — Removed Switchbrew format pages

**Research ID:** R-0008
**Date:** 2026-11-04
**Subsystem:** research infrastructure / assets
**Source:** Switchbrew wiki
**Reference URL:** https://switchbrew.org/wiki/File_formats
**Technical question:** Are the mandatory asset-format wiki pages still available?

**Observed behavior:** `File_formats`, `PFS0`, `RomFs`, `BNTX`, `BFRES`, `AAMP`
and `SARC` returned HTTP 404. The NCA page still contains PFS0 and RomFS context.
`NCA`, `NSO`, `NRO`, `ExeFS`, `NACP_Format`, `MOD` and `GPU` returned 200.

**Verified facts:** The removed pages are unavailable at the recorded URLs on the
check date. The original URLs are preserved in `REFERENCE_SOURCES.md`.

**Hypothesis:** No hypothesis about why they were removed. No content from them
is being claimed.

**Implementation implications:** Asset format work must use independently
verified implementation references and synthetic fixtures. The NCA page is a
verified supplementary source for PFS0/RomFS context, not a replacement for the
removed pages.

**Test required:** Re-check availability at the next phase gate; record any
archive recovery separately.

**Status:** VERIFIED (unavailability); asset format details remain UNVERIFIED.

---

## R-0009 — LibHac availability

**Research ID:** R-0009
**Date:** 2026-11-04
**Subsystem:** research infrastructure / binary
**Source:** GitHub
**Reference URL:** https://github.com/Thealexbarney/LibHac
**Technical question:** Is the canonical LibHac repository still available as a
format reference?

**Observed behavior:** The URL and the `LibHac/LibHac` candidate returned 404.
`Thealexbarney/LibHacTools` and `LibHac.Nand` still return 200, but they are not
the complete library. No verified canonical replacement was found.

**Verified facts:** The original repository is unavailable on the check date.

**Hypothesis:** The project may have been removed or relocated without a public
canonical URL. Unverified forks are not accepted as authority.

**Implementation implications:** Re-derive format behavior from Switchbrew,
libnx, switch-tools and tests. Record each future claim independently.

**Test required:** None for Phase 0; re-check before Phase 1 format expansion.

**Status:** VERIFIED (unavailability); replacement UNVERIFIED.
