# Third-party dependencies

NXRelinker does not vendor third-party source trees without a documented reason.
Current build dependencies are resolved by CMake.

| Dependency | Version | How resolved | License | Purpose |
| --- | --- | --- | --- | --- |
| GoogleTest | v1.15.2 | System package or pinned FetchContent archive with SHA-256 | BSD-3-Clause | Unit tests |

Planned but not yet integrated: LLVM/Clang libraries for ARM64 decoding and
code generation, Vulkan headers/loader, SPIRV-Tools and Vulkan Validation Layers.
Each will be added only after its license, ABI and build integration are reviewed
and recorded in an ADR.

Reference-only projects listed in `docs/research/REFERENCE_SOURCES.md` are not
dependencies. Their code is not copied into this repository. Incompatible
licenses (for example GPL) prohibit direct code reuse in the MIT-licensed core
unless a separate integration decision is made.

## Adding a dependency

1. Document the need and alternatives in `docs/architecture/DECISIONS.md`.
2. Verify the license and record it here.
3. Pin an exact version and, for downloaded archives, a SHA-256 hash.
4. Exclude third-party targets from first-party warning-as-error settings.
5. Add a reproducibility check to CI if the dependency affects the build.
