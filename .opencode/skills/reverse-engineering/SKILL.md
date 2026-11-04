---
name: reverse-engineering
description: Use for Ghidra/IDA analysis, ARM64 disassembly, function identification, cross-references, call graphs, data flow and ABI recovery on Switch binaries.
---

# Reverse Engineering

## Purpose

Recover testable facts from Switch binaries using Ghidra, IDA, ARM64 disassembly
and manual analysis. The output feeds parsers, recompiler semantics and runtime
contracts, but decompiler output is evidence, not truth.

## When to use

- Identifying function boundaries or call graphs.
- Mapping imports/exports or service calls.
- Recovering data structures used by a format.
- Validating a recompiler or relocation hypothesis.
- Investigating an unknown code or data region.
- Comparing multiple builds or versions.

## Required sources

- `https://github.com/Adubbz/Ghidra-Switch-Loader`
- `https://github.com/pgarba/SwitchIDAProLoader` (replacement for the unavailable
  `roblabla/ida-switch-loader`)
- `https://ghidra-sre.org/`
- `https://github.com/NationalSecurityAgency/ghidra`
- `https://github.com/mandiant/capa`
- `https://dogbolt.org/` (only for non-sensitive files)
- `https://github.com/Atmosphere-NX/Atmosphere` for Horizon behavior context
- ARM architecture manuals for instruction meaning
- `docs/research/REFERENCE_SOURCES.md`, `RESEARCH_LOG.md`

Emulator and decompiler output is supporting evidence. It must not be the sole
authority for a hardware or OS contract.

## Operating rules

- Analyze copies. Never modify original game files.
- Never upload commercial binaries, keys, dumps or save data to web services.
- Do not implement or document DRM circumvention, key extraction or console
  authorization bypass.
- Record the tool, version, loader, base address and analysis options.
- Distinguish code from data; do not let a disassembler's heuristic become a
  fact.
- Keep decompiler pseudocode out of production source. Re-express behavior and
  test it.
- If a function or structure is reconstructed, add a synthetic fixture or a
  differential test.

## Workflow

1. **Set up the loader.** Use the correct NSO/NRO/ELF loader and base address.
2. **Map the image.** Record segments, entry points, imports, exports and
   relocation targets.
3. **Find functions.** Start from exports/imports, call references and known
   service stubs. Record confidence.
4. **Build cross-references.** Identify callers, callees, data references and
   string references.
5. **Recover structures.** Validate field widths, alignment and lifecycle across
   multiple use sites.
6. **Hypothesize behavior.** Label it `INVESTIGATING`; do not promote to fact.
7. **Create a test.** Use a synthetic input or a controlled differential case.
8. **Update docs.** Add the finding, evidence and status to the research log.

## Evidence quality

| Evidence | Strength |
| --- | --- |
| ARM manual + reproducible test | Strong |
| Official/primary format documentation | Strong |
| Multiple independent implementations agree | Medium |
| Single implementation source | Weak |
| Decompiler pseudocode | Weak until tested |
| Community post | Weak; corroboration required |
| Guessed field name | Not evidence |

## Common mistakes

- Treating decompiler variable names as original semantics.
- Assuming every `BL` target is a function.
- Ignoring ARM64 pointer authentication or branch islands if present.
- Forgetting that 32-bit writes zero-extension semantics affect analysis.
- Using the wrong loader or base address for an NSO/NRO.
- Uploading proprietary files to a public decompiler service.
- Reporting a structure layout from one use site.
- Leaving a reconstructed layout undocumented and untested.
- Mixing analysis hypotheses with implemented facts.

## Checks

- [ ] Tool, version, loader and base address recorded.
- [ ] Every claimed function boundary has evidence.
- [ ] Data/code distinction is justified.
- [ ] Structures are validated at multiple use sites or marked provisional.
- [ ] No commercial files or keys added to the repository.
- [ ] Findings recorded with status in the research log.
- [ ] Recovered behavior has a synthetic test before implementation.

## Completion criteria

- The question is answered with cited or reproducible evidence.
- Confidence and unresolved ambiguity are stated.
- Implementation consequences are listed.
- Tests or a test plan exist.
- No proprietary artifact is committed.

## Related tests

- Synthetic binaries from `tests/binary/synthetic_builders.h`.
- Future ARM64 decode/semantic tables under `tests/arm64/`.
- Differential tests under `tests/recompiler/`.
