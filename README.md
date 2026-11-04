# NXRelinker

NXRelinker is a PC-side Nintendo Switch binary analysis, relinking and native
recompilation project. It targets first-generation Switch executables
(`NSO`, `NRO`, ELF64 AArch64) and, over time, native Windows/Linux output backed
by a Horizon compatibility runtime, an NVN compatibility layer and Vulkan.

NXRelinker is **not** an emulator. It does not emulate the Tegra X1, does not
interpret ARM64 as its final execution strategy and does not embed emulator
code. The target is native x86-64 code with translated platform semantics.

## Current status: Phase 0 — Repository Foundation

The repository currently implements:

- Strict signature detection:
  - NSO: `NSO0` at offset `0x0`
  - NRO: `NRO0` at offset `0x10`, after the Rocrt header
  - ELF: `\x7FELF` at offset `0x0`
- NSO header parsing and structural validation:
  - Text/rodata/data file and memory ranges
  - Compression flags and compressed-size fields
  - Module ID, BSS size and module-name range
  - Embedded/dynamic-string/dynamic-symbol offsets
  - Overlap and out-of-range diagnostics
- NRO/Rocrt header parsing and structural validation:
  - Segment memory ranges and overlap checks
  - Total-size and trailing-asset diagnostics
  - Additional `NRA0` header presence checks
  - Module ID, BSS size and dynamic-table offsets
- ELF64 AArch64 header validation:
  - Class, endianness, machine, header/table bounds
- `nxinspect` text and JSON reports with severity-coded diagnostics
- Synthetic unit tests, CTest integration, GitHub Actions CI and sanitizer
  support

Not implemented yet: disassembly, CFG, ARM64 semantics, x86-64 code generation,
relocations, symbol/import/export resolution, module linking, Horizon services,
NVN, Vulkan, shaders, assets or executable generation. The CLI does not produce
a runnable game and will say so.

## Build

Linux/macOS:

```bash
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j"$(nproc)"
ctest --test-dir build/release --output-on-failure
```

Debug:

```bash
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j"$(nproc)"
ctest --test-dir build/debug --output-on-failure
```

Presets:

```bash
cmake --preset release
cmake --build --preset release
ctest --preset release
```

Windows: Visual Studio 2022 with Ninja or the Visual Studio generator. See
[`docs/development/BUILDING.md`](docs/development/BUILDING.md).

## Usage

```bash
./build/release/nxinspect game.nso
./build/release/nxinspect --format json game.nso
./build/release/nxinspect --format json --strict game.nro
```

Exit codes:

| Code | Meaning |
| --- | --- |
| `0` | Inspection completed without errors |
| `1` | Malformed or unsupported input |
| `2` | Invalid command line |
| `3` | Input could not be read |
| `4` | Warnings found while `--strict` was enabled |

Example text report:

```text
File: game.nso
Format: NSO0
File size: 4096 bytes (0x1000)
Signature offset: 0x0
Version: 0
Flags: 0x1
Module ID: ...
BSS size: 0 bytes (0x0)
Segments:
  text: memory=0x0 memory_size=4096 file=0x100 file_size=2048 compressed
  ...
Diagnostics: 0 error(s), 0 warning(s), 0 info
```

The JSON report contains `schema_version`, file metadata, format-specific header
fields, segment metadata, diagnostics and a summary. It is generated without an
external JSON dependency.

## Repository layout

```text
include/nxrelinker/   public headers by subsystem
src/                  implementation by subsystem
tools/nxinspect/      Phase 0 inspector CLI
tests/binary/         synthetic executable-format tests
tests/unit/           core and diagnostics unit tests
docs/                 architecture, formats, research, development
.opencode/            specialist agents and skills
.github/workflows/    Linux and Windows CI
```

## Roadmap

| Phase | Scope |
| --- | --- |
| 0 | Foundation, repository tooling, NSO/NRO/ELF signature and header checks |
| 1 | Binary analysis: sections, MOD0, relocations, imports/exports |
| 2 | ARM64 decoder, disassembler, CFG, function boundaries |
| 3 | Native recompiler and differential tests |
| 4 | Relinker, module loading and native executable generation |
| 5 | Horizon compatibility runtime |
| 6 | NVN compatibility layer, shaders and Vulkan |
| 7 | Game compatibility, profiling and optimization |

See [`docs/ROADMAP.md`](docs/ROADMAP.md) and
[`docs/architecture/ARCHITECTURE.md`](docs/architecture/ARCHITECTURE.md).

## Research and honesty rules

All technical claims must be traceable to
[`docs/research/REFERENCE_SOURCES.md`](docs/research/REFERENCE_SOURCES.md) and
[`docs/research/RESEARCH_LOG.md`](docs/research/RESEARCH_LOG.md). A hypothesis is
not a fact. A successful compilation is not proof of correctness. The project
does not include DRM circumvention, key extraction, console authorization
bypass or copyrighted game data.

## License

MIT. See [`LICENSE`](LICENSE). Third-party references and dependencies retain
their own licenses; see [`third_party/README.md`](third_party/README.md).
