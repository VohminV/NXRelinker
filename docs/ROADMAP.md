# Roadmap

The phase gates come from the repository contract. A phase is complete only when
its implementation, tests, documentation and CI evidence are present.

## Phase 0 — Repository Foundation (current)

- Repository structure and agent/skill instructions.
- CMake/Ninja/CTest/CI for Linux and Windows.
- Source registry, research log and ADRs.
- Signature detection for NSO/NRO/ELF.
- NSO/NRO/ELF header parsing with structural diagnostics.
- `nxinspect` text and JSON reports.
- Synthetic tests, sanitizer preset and build documentation.

**Exit criteria:** clean Release and Debug builds, all tests pass, CI is defined
for Linux and Windows, and no feature is claimed beyond what is tested.

## Phase 1 — Binary Analysis

- ELF64 program headers and section headers.
- NSO/NRO segment extraction, decompression (LZ4/ZBIC as verified).
- MOD0, dynamic tables, dynamic symbols and strings.
- Relocation parsing and symbol resolution.
- Import/export tables and dependency metadata.
- Synthetic multi-segment and multi-module fixtures.

**Exit criteria:** structurally correct analysis of synthetic and lawfully
obtained test images, with negative-path tests and a documented JSON schema.

## Phase 2 — ARM64 Analysis

- Instruction decoder and disassembler.
- AArch64 instruction groups used by Switch code.
- Basic-block and CFG construction.
- Function boundary detection.
- ABI and register/stack analysis.

**Exit criteria:** supported instruction set is decoded and tested against a
primary architecture reference; unsupported encodings are diagnosed, not guessed.

## Phase 3 — Native Recompiler

- ARM64 IR and semantics.
- x86-64 lowering and register mapping.
- Flags, branches, memory ordering and atomics.
- SIMD/NEON and floating point.
- Calling convention translation.
- Differential tests for every supported instruction family.

**Exit criteria:** differential tests compare observable behavior of original
ARM64 and generated x86-64 code.

## Phase 4 — Relinker

- Module loader and address remapping.
- Relocation application and import/export resolution.
- Dependency graph and symbol management.
- Native executable/module emission.

**Exit criteria:** a synthetic multi-module ARM64 program runs after conversion to
x86-64.

## Phase 5 — Horizon Runtime

- Process/thread/handle model.
- IPC serialization and service dispatch.
- Memory and synchronization primitives.
- FS, HID, audio, time, VI and applet services in priority order.
- Per-service documentation, error semantics and tests.

**Exit criteria:** a test application runs with the implemented service subset.

## Phase 6 — Graphics

- NVN device, queue, command buffer and memory model.
- Texture/buffer/sampler/render-target state.
- GPU command translation.
- Shader analysis, IR and SPIR-V translation.
- Vulkan 1.3 backend with Validation Layers.

**Exit criteria:** a test 3D scene with textures, depth testing and synchronization
renders correctly and validation-clean.

## Phase 7 — Game Compatibility

- Per-application dependency analysis.
- Compatibility matrix and documented limitations.
- Profiling, code cache and runtime optimization.
- Regression and performance testing.

**Exit criteria:** reproducible run of a selected lawful test application with a
documented compatibility level.

## Guardrails

- No phase may be skipped by embedding an emulator or interpreter.
- No fake executable may be presented as a working game.
- Optimization follows correctness and measurements.
- Research gaps are logged, not hidden.
