# Architecture Decisions

Each decision records context, alternatives, consequences and status. Public API
or subsystem-boundary changes require an entry here before implementation.

## ADR-0001: C++20 with CMake and Ninja

**Status:** Accepted (2026-11-04)

**Context:** The project needs a portable, strongly typed implementation language
for binary parsing, recompilation and GPU translation on Windows and Linux.

**Decision:** Use C++20 as the implementation language, CMake ≥ 3.24 as the build
system and Ninja as the default generator. MSVC, GCC and Clang are supported.

**Alternatives considered:**

- C++17: rejects `std::span` and other standard facilities used by the parser.
- Rust: strong safety, but the planned LLVM MC/ORC and Vulkan integration path
  and the expected contribution pool favor C++.
- Custom build system: unnecessary maintenance burden.

**Consequences:** Build configuration must remain generator-agnostic. Compiler
warnings are errors for first-party targets. C++23 features are not assumed.

## ADR-0002: GoogleTest via pinned FetchContent

**Status:** Accepted (2026-11-04)

**Context:** Tests must run on Linux and Windows CI without installing a system
test framework.

**Decision:** Prefer a system GoogleTest when available. Otherwise fetch
GoogleTest v1.15.2 from the official release archive and verify SHA-256
`7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926`.
GoogleTest is BSD-3-Clause.

**Alternatives considered:**

- Vendoring GoogleTest into the repository: adds third-party code and update
  noise.
- Catch2: viable, but GoogleTest is the most common CTest/CI choice here.
- Hand-written test runner: insufficient for discovery and fixtures.

**Consequences:** First configure needs network access unless GoogleTest is
installed system-wide. CI must be able to fetch the archive. Dependency updates
require changing the version and hash together.

## ADR-0003: Foundation implements only verified header parsing

**Status:** Accepted (2026-11-04)

**Context:** The prompt requests a full relinker, but Phase 0 is explicitly a
repository foundation. Claiming recompiler or runtime functionality before it
exists would violate the project's truthfulness rules.

**Decision:** Phase 0 implements signature detection plus NSO, NRO and ELF64
header parsing with structural diagnostics. It does not implement decompression,
relocation application, disassembly, recompilation, runtime services, NVN or
executable generation. `nxinspect` reports only what it can prove.

**Alternatives considered:**

- Scaffold empty recompiler/runtime APIs: creates fake boundaries and invites
  placeholder implementations.
- Implement a minimal interpreter: explicitly out of scope and prohibited as a
  substitute for native recompilation.

**Consequences:** CLI surface is intentionally small. Phase 1 starts only after
Phase 0 CI is green. Future docs must not describe unimplemented stages as
working.

## ADR-0004: Diagnostics are a separate subsystem

**Status:** Accepted (2026-11-04)

**Context:** Parsers produce recoverable findings, and text/JSON output must stay
consistent.

**Decision:** `nxrelinker::diagnostics` owns `Severity`, `Diagnostic`,
`DiagnosticBag`, JSON escaping and the small JSON writer. It has no dependency on
`binary`. Parsers return `ParseResult<T>` with a value and a diagnostic bag.

**Alternatives considered:**

- Exceptions for malformed input: complicates recoverable reporting and mixes
  control flow with data validation.
- Parser-specific diagnostic structs: duplicates serialization and severity
  semantics.

**Consequences:** Diagnostics must remain format-agnostic. Format-specific
meaning is expressed through stable diagnostic code strings. JSON schema changes
require review.

## ADR-0005: No emulator code or architecture reuse

**Status:** Accepted (2026-11-04)

**Context:** Ryujinx, yuzu and related projects are useful behavioral references,
but NXRelinker is a different kind of system.

**Decision:** Emulator repositories are read-only references for observable
behavior and format clues. Do not copy source, architectural layering, generated
shader code or assets. Do not embed an emulator. Do not use emulator behavior as
the sole authority for a hardware or OS contract.

**Alternatives considered:**

- Forking an emulator as a starting point: violates the project identity and
  would produce an emulator rather than a native recompiler.

**Consequences:** NXRelinker must independently specify IPC, service semantics,
GPU state and shader translation, backed by primary documentation and tests.

## ADR-0006: In-tree JSON writer for the inspection report

**Status:** Accepted (2026-11-04)

**Context:** `nxinspect --format json` needs stable machine-readable output. A
full JSON dependency is not justified for one small writer.

**Decision:** Implement a bounds-safe JSON writer and string escaper in
`diagnostics`. It handles objects, arrays, strings, unsigned numbers, booleans and
null. Numbers are non-negative integers, which covers the current schema.

**Alternatives considered:**

- Add nlohmann/json: extra dependency, compile-time cost and license review for a
  small need.
- Hand-concatenate JSON in the CLI: harder to test and easy to corrupt.

**Consequences:** If future reports need signed/floating-point values or
incremental streaming, revisit the writer API via an ADR. The current schema is
`schema_version: 1`.

## ADR-0007: Fatal versus recoverable parse results

**Status:** Accepted (2026-11-04)

**Context:** Users need detailed diagnostics even for malformed files, but some
inputs cannot be parsed at all.

**Decision:** Fatal errors (short input, invalid signature, unsupported ELF class
or endianness) return a `ParseResult` with no value. Recoverable problems
(out-of-range optional tables, overlaps, unknown flags, size mismatches) return a
value plus error/warning diagnostics. `nxinspect` exits non-zero when any error
diagnostic is present.

**Alternatives considered:**

- Fail on the first diagnostic: loses useful structural context.
- Always return a value: encourages callers to ignore fatal errors.

**Consequences:** Parsers must consistently classify findings. Tests cover both
fatal and recoverable paths. CLI exit codes distinguish parse errors from I/O and
usage errors.
