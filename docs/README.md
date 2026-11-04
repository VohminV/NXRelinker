# Documentation Index

| Document | Purpose |
| --- | --- |
| [`ROADMAP.md`](ROADMAP.md) | Phase plan and completion criteria |
| [`architecture/ARCHITECTURE.md`](architecture/ARCHITECTURE.md) | Subsystem boundaries, pipeline and parser design |
| [`architecture/DECISIONS.md`](architecture/DECISIONS.md) | Architecture decision records |
| [`formats/NSO.md`](formats/NSO.md) | Verified NSO header notes and parser behavior |
| [`formats/NRO.md`](formats/NRO.md) | Verified NRO/Rocrt notes and parser behavior |
| [`formats/ELF.md`](formats/ELF.md) | ELF64 AArch64 header notes and parser behavior |
| [`research/REFERENCE_SOURCES.md`](research/REFERENCE_SOURCES.md) | Source registry with verification status and licenses |
| [`research/RESEARCH_LOG.md`](research/RESEARCH_LOG.md) | Findings, hypotheses and open questions |
| [`development/BUILDING.md`](development/BUILDING.md) | Linux, Windows, preset and sanitizer builds |
| [`development/TESTING.md`](development/TESTING.md) | Test layout, coverage and adding tests |

## Rules for documentation

- State implemented behavior, not intended behavior, unless the section is
  clearly marked as future work.
- Cite the source and access date for every format or platform claim.
- Keep unavailable source URLs visible and record verified alternatives.
- Record hypotheses in the research log; do not promote them to facts in format
  documents.
- Update the research log and an ADR when a contract or public API changes.
