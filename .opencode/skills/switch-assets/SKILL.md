---
name: switch-assets
description: Use for BNTX, BFRES, SARC, AAMP, RomFS, PFS0, NACP and ExeFS parsing, validation, extraction and conversion in NXRelinker.
---

# Switch Assets

## Purpose

Parse and validate first-generation Switch container and resource formats while
keeping extraction, validation and conversion separate. Never treat a file
extension or a single tool's behavior as sufficient proof of a format.

## When to use

- Implementing RomFS, PFS0, NACP or ExeFS parsing.
- Implementing BNTX, BFRES, SARC or AAMP parsing.
- Adding texture/model/audio conversion.
- Building synthetic asset fixtures.
- Investigating unknown chunks or version differences.

## Current source status

The historical standalone Switchbrew pages for `File_formats`, `PFS0`, `RomFs`,
`BNTX`, `BFRES`, `AAMP` and `SARC` returned **HTTP 404** on 2026-11-04. They are
retained in `REFERENCE_SOURCES.md`. The `NCA` page still contains verified PFS0
and RomFS context. Do not claim removed page content as verified.

Verified implementation references (as of 2026-11-04):

- `https://github.com/KillzXGaming/Switch-Toolbox` — GPL-3.0
- `https://github.com/KillzXGaming/BfresLibrary` — MIT
- `https://github.com/KillzXGaming/AampLibrary` — license not detected, verify
- `https://github.com/ThemezerNX/SarcLib` — MIT
- `https://github.com/aboood40091/SarcLib` — GPL-3.0
- `https://github.com/aboood40091/BNTX-Extractor` — GPL-3.0
- `https://github.com/nicoboss/nsz` — NSP/XCI container reference, license verify
- `https://github.com/blawar/nut` — PFS0/NSP handling, GPL-3.0
- `https://github.com/switchbrew/libnx` — NACP structures, ISC

Do not copy GPL code into the MIT core without a separate licensing decision.

## Layers

Keep these stages separate:

1. **Container parsing:** RomFS, PFS0, NACP, ExeFS.
2. **Resource parsing:** BNTX, BFRES, SARC, AAMP.
3. **Resource validation:** offsets, counts, string tables, alignment, bounds.
4. **Resource extraction:** copy or map bytes without modifying originals.
5. **Conversion:** texture decode, model conversion, audio conversion.

A converter must not own container policy. A parser must not silently rewrite a
resource.

## Workflow

1. Read the current asset code and source registry.
2. Identify the exact layer and version you are implementing.
3. Document header offsets, endianness, alignment, string tables and unknown
   fields from a primary or independently verified reference.
4. Build a minimal synthetic fixture:
   - correct magic and version;
   - one node/entry per structural concept;
   - explicit offsets and sizes;
   - no commercial content.
5. Implement bounds-checked parsing with explicit handling of unknown chunks.
6. Add truncation, bad-magic, out-of-range, duplicate and empty-table tests.
7. Implement extraction under a separate API.
8. Implement conversion only after validation passes.
9. Run Release, Debug and sanitizer tests.

## Common mistakes

- Assuming the wiki page still exists or that an emulator's parser is definitive.
- Ignoring alignment and string-table offsets.
- Dropping unknown chunks instead of preserving or rejecting them.
- Treating a version field as always zero.
- Using a texture decoder before validating swizzle/tiling.
- Modifying input files in place.
- Adding extracted commercial assets to tests or fixtures.
- Mixing NACP metadata with NCA container headers without evidence.
- Copying GPL implementation code into the MIT library.

## Checks

- [ ] Format magic and version handling documented.
- [ ] All offsets/counts are bounds-checked with overflow-safe math.
- [ ] String tables and alignment validated.
- [ ] Unknown chunks preserved or explicitly rejected.
- [ ] Synthetic fixtures cover valid and malformed inputs.
- [ ] Extraction works on copies; originals untouched.
- [ ] Conversion has format-specific tests.
- [ ] License of every reference implementation recorded.

## Completion criteria

- The parser and validator are implemented and tested.
- No commercial data is required.
- Unsupported versions/features fail explicitly.
- Container/resource/conversion boundaries are respected.
- Documentation records evidence and known gaps.

## Related tests

Future layout:

- `tests/assets/` — container/resource parser tests.
- `tests/assets/conversion/` — texture/model/audio conversion tests.
