---
name: performance-optimization
description: Use for CPU profiling, recompiled code throughput, allocation, cache locality, Vulkan profiling, multithreading, frame pacing and benchmark methodology in NXRelinker.
---

# Performance Optimization

## Purpose

Make native execution fast without sacrificing correctness. Optimize only after
behavior is validated, measure before and after, and keep benchmark methodology
reproducible.

## When to use

- Profiling parser, linker, runtime or recompiled code.
- Reducing function compilation time or JIT overhead.
- Improving register allocation, inlining, dead-code elimination or constant
  propagation.
- Reducing allocations or improving cache locality.
- Profiling Vulkan/GPU behavior and frame pacing.
- Adding a benchmark or performance regression test.

## Required sources

- LLVM optimization and ORC documentation:
  `https://llvm.org/docs/CodeGenerator.html`,
  `https://llvm.org/docs/ORCv2.html`
- Arm architecture manuals for instruction costs and memory ordering.
- Vulkan specifications and Validation Layers.
- `docs/research/REFERENCE_SOURCES.md`, `RESEARCH_LOG.md`.
- Existing benchmark harness and profiling notes in `docs/benchmarks/` when
  present.

## Non-negotiable rules

- Do not optimize code whose correctness is not established.
- Do not trade defined behavior for undefined behavior.
- Do not enable fast-math or relaxed memory ordering without a verified semantic
  argument and tests.
- Do not benchmark Debug builds and present them as production results.
- Do not compare runs with different inputs, thermal states or driver versions
  without recording it.
- Do not add a persistent cache without invalidation, versioning and corruption
  tests.
- Do not claim a speedup without before/after numbers and a reproducible method.

## Measurement workflow

1. **Define the metric.** Examples: instructions/second, translation blocks/
   second, function compile time, allocation count, frame time, 99th percentile
   frame time.
2. **Establish a baseline.** Release build, fixed input, warmup, multiple runs,
   record compiler/CPU/GPU/driver versions.
3. **Profile.** Use sampling or instrumentation to find the actual hotspot.
4. **State a hypothesis.** Explain why the change should affect the measured
   metric.
5. **Change one variable.** Keep semantics and tests intact.
6. **Re-measure.** Use the same method and report variance.
7. **Validate correctness.** Run the full relevant test suite, including
   differential tests.
8. **Record.** Update the benchmark log and, if applicable, an ADR.

## Areas and constraints

### Recompiled code

- Register allocation must preserve ABI and NZCV semantics.
- Inlining must respect function boundaries and relocation assumptions.
- SIMD optimization requires lane-level differential tests.
- Persistent code caches require version keys over CPU features, code hash,
  recompiler version and relocation layout.

### Runtime

- Allocation changes must preserve ownership, lifetime and thread safety.
- Lock-free code requires a memory-ordering argument and stress tests.
- Thread pools must not change observable scheduling semantics assumed by tests.

### Graphics

- Validate every new path with Vulkan Validation Layers before profiling.
- Separate CPU translation cost from GPU execution cost.
- Record resolution, format, driver, device and scene.
- Frame pacing changes must not hide missing synchronization.

## Common mistakes

- Optimizing a cold path because it looks expensive.
- Benchmarking with a profiler attached or with Debug assertions.
- Reporting one run with no warmup.
- Changing multiple variables at once.
- Using `-ffast-math` to fix NaN or rounding differences.
- Adding a cache without invalidation.
- Assuming the host memory model matches AArch64.
- Ignoring compile-time/JIT overhead in throughput claims.
- Letting a benchmark become a correctness shortcut.

## Checks

- [ ] Correctness tests pass before and after.
- [ ] Baseline and result use identical methodology.
- [ ] Environment and versions are recorded.
- [ ] Variance and sample count are reported.
- [ ] No undefined-behavior or memory-ordering shortcut.
- [ ] Cache invalidation is tested.
- [ ] Benchmarks are reproducible from documented commands.
- [ ] Results are stored outside tracked build directories.

## Completion criteria

- The change is justified by measurements.
- The metric improved without regressing correctness or other key metrics.
- Methodology and limitations are documented.
- Relevant tests and differential tests pass.
- No performance claim exceeds the evidence.

## Related tests

- Correctness suites for the affected subsystem.
- Differential tests for recompiler changes.
- Vulkan Validation Layers for graphics changes.
- Future benchmarks under `tests/benchmarks/` or `docs/benchmarks/`.
