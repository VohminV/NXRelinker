# NXRelinker — Agent Instructions

This file is the shared contract for every human contributor and every OpenCode
agent working in this repository.

## 1. Project identity

NXRelinker is a PC-side Nintendo Switch **relinker and native recompiler**.

Target pipeline:

```text
Switch executable/container
        -> binary analysis
        -> module relinking and symbol resolution
        -> ARM64 semantic analysis
        -> x86-64 code generation
        -> Horizon compatibility runtime
        -> NVN compatibility layer
        -> Vulkan renderer
        -> native Windows/Linux executable
```

It is **not** an emulator and must not become one. It does not emulate the
Tegra X1, does not interpret ARM64 as the final execution strategy, and does not
embed an emulator as a shortcut.

Platform scope is Nintendo Switch generation 1 only: Tegra X1, Cortex-A57 /
AArch64, GM20B Maxwell, Horizon OS, NVN, NSO/NRO. Do not introduce PlayStation,
Xbox, Tegra X2, T239 or Nintendo Switch 2 assumptions.

## 2. Current status and truthfulness

Current milestone: **Phase 0 — Repository Foundation**.

Implemented and tested:

- Repository structure, build system, CI configuration and agent/skill docs.
- Strict signature detection for NSO (`NSO0` at 0x0), NRO (`NRO0` at 0x10) and
  ELF (`\x7FELF` at 0x0).
- NSO header parsing, structural range/overlap diagnostics, compression flags,
  module ID, dynamic-table offsets and segment metadata.
- NRO/Rocrt header parsing, memory-segment validation, flag diagnostics,
  additional-header and trailing-asset diagnostics.
- ELF64 AArch64 header validation.
- Text and JSON inspection reports via `nxinspect`.

Not implemented yet: relocation resolution, symbol/import/export tables,
disassembly, CFG, recompiler, linker, Horizon runtime, NVN, assets, or native
executable generation. Never describe these as working.

The ground truth for what works is the source plus the test suite, not an agent
summary.

## 3. Non-negotiable engineering rules

1. Inspect existing code before editing it. Preserve unrelated behavior.
2. Do not invent APIs, file formats, struct offsets, service calls or NVN
   enums. Verify against a primary source or a reference implementation and
   record the evidence.
3. Do not leave fake implementations, placeholder return values or “TODO”
   paths in features advertised as complete.
4. Do not suppress compiler errors/warnings to make a build pass.
5. A successful compile is not proof of correctness. Add or update tests.
6. Fix root causes, not symptoms. Do not add global hacks for local defects.
7. Prefer focused modules over monolithic translation units.
8. Do not add a dependency without a documented reason, license check and
   reproducible version pin.
9. Never commit keys, decrypted commercial binaries, extracted commercial assets,
   console dumps, save files or proprietary SDK code.
10. Never implement DRM circumvention, key extraction, console authorization
    bypass or piracy tooling.
11. Do not copy emulator code or architecture. Emulator repositories are
    behavioral references only.
12. Public API changes require an architect decision recorded in
    `docs/architecture/DECISIONS.md`.

## 4. Repository map

```text
include/nxrelinker/        public headers, grouped by subsystem
src/                       implementation; same subsystem names as headers
tools/nxinspect/           implemented CLI for Phase 0
tests/unit/                subsystem-neutral tests
tests/binary/              executable-format tests and synthetic builders
tests/fixtures/synthetic/  generated or purely synthetic test data only
docs/architecture/         architecture and ADRs
docs/formats/              verified format notes for implemented parsers
docs/research/             source registry and research log
docs/development/          build and test instructions
cmake/                     reusable CMake modules
.opencode/agents/          specialist agent definitions
.opencode/skills/          specialist skill instructions
```

Planned subsystems (`arm64`, `recompiler`, `linker`, `runtime`, `horizon`,
`graphics`, `shaders`, `assets`, `diagnostics`) will be created only when their
implementation begins. Empty placeholder directories are not committed.

## 5. Build and test commands

Linux/macOS (GCC or Clang):

```bash
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j"$(nproc)"
ctest --test-dir build/release --output-on-failure

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

Sanitizers (GCC or Clang with runtime installed):

```bash
cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DNXRELINKER_ENABLE_SANITIZERS=ON
cmake --build build/asan -j"$(nproc)"
ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
ctest --test-dir build/asan --output-on-failure
```

Windows: use Visual Studio 2022 with Ninja or the Visual Studio generator.
See `docs/development/BUILDING.md`.

## 6. Architecture rules

- `binary` owns format parsing and structural validation. It must not know about
  execution, Horizon services or Vulkan.
- `diagnostics` owns diagnostic value types and serialization. It must not depend
  on binary formats.
- Future `linker` owns loading and resolution. It must not contain recompiler
  logic.
- Future `recompiler` owns ARM64 semantics and x86-64 emission. It must not
  contain Horizon service policy.
- Future `runtime`/`horizon` own service semantics. They must not parse NSO
  headers themselves.
- Future `graphics`/`shaders` own NVN state and shader translation. They must
  not be hidden inside the runtime or recompiler.
- Parsers must be bounds-checked, deterministic and safe on untrusted input.
- Public headers use the `nxrelinker` namespace and subsystem namespaces.

## 7. Research rules

- All sources are registered in `docs/research/REFERENCE_SOURCES.md`.
- New findings go into `docs/research/RESEARCH_LOG.md` using the required
  template and status values.
- Important cross-module decisions go into `docs/architecture/DECISIONS.md`
  with alternatives and consequences.
- A hypothesis is not a fact. Mark it `UNVERIFIED` or `INVESTIGATING` until a
  primary source or a reproducible test confirms it.
- If a referenced URL is dead, record the failure, retain the original URL, and
  list a verified alternative. Do not silently replace history.
- Community posts and emulator source are supporting evidence, never the sole
  authority for a hardware or OS contract.

## 8. Testing requirements

Every behavioral change needs an automated test. Minimum expectations:

- Binary parsers: valid, truncated, invalid-magic, out-of-range and overlap
  cases. Tests use synthetic in-memory files, never commercial game data.
- ARM64/recompiler: differential tests comparing observable ARM64 behavior with
  generated x86-64 behavior.
- Runtime: IPC serialization, handle semantics, synchronization and error paths.
- Graphics: Vulkan Validation Layers and SPIR-V validation must be clean for
  newly added paths.
- No test may require a console, keys or copyrighted files.

Definition of done for a task:

1. Code compiles in Release and Debug.
2. Relevant tests pass locally.
3. Warnings are not suppressed.
4. Documentation and research log are updated when facts or contracts change.
5. Limitations are reported honestly.

## 9. Agent routing

- Architecture, module boundaries, dependencies: `architect`.
- NSO/NRO/ELF, relocations, symbols, imports/exports: `binary-analysis`.
- ARM64 decode, CFG, IR, x86-64 emission, ABI, differential tests:
  `arm64-recompiler`.
- Horizon IPC, services, threads, memory, FS, HID, audio: `horizon-runtime`.
- NVN, Vulkan, shaders, SPIR-V, GPU memory: `graphics-engineer`.
- BNTX, BFRES, SARC, AAMP, RomFS, textures, models: `asset-engineer`.
- CTest, CI, regression, benchmarks, cross-platform validation: `test-engineer`.

Agents must load the matching skill before working in a specialist area. The
architect must be consulted before changing a public API or crossing a subsystem
boundary.

## 10. Required task report

After each completed task, report in this format:

```text
TASK:
ROOT CAUSE:            (for fixes)
IMPLEMENTATION:        files created/modified
TECHNICAL DETAILS:     how it works
REFERENCES:            verified sources
TESTS:                 added/updated tests
BUILD:                 Release/Debug result
BENCHMARK:             measurements, if applicable
LIMITATIONS:           what remains unsupported
NEXT STEP:             one concrete next action
```

Do not claim a successful build or test run that did not occur. Do not hide
failing tests.

## 11. Phase gate

Phase 1 (binary analysis expansion: ELF segments/sections, MOD0, dynamic tables,
relocations, imports/exports) must not start until Phase 0 builds and tests
cleanly on Linux and Windows CI. Phase 0 is complete only when the repository,
agents, skills, source registry, build, tests and CI are all present and
verified.
