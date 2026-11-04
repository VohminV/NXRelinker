# Building NXRelinker

## Requirements

| Component | Minimum | Notes |
| --- | --- | --- |
| CMake | 3.24 | `CMakePresets.json` uses version 6 |
| Compiler | GCC 13+, Clang 16+ or MSVC 19.3x | C++20 required |
| Generator | Ninja | Visual Studio generator is supported on Windows |
| Network | First configure only | Required when GoogleTest is not installed system-wide |

GoogleTest v1.15.2 is fetched from the official release archive and verified with
SHA-256 when a system package is absent. The archive is not committed.

## Linux/macOS

Release:

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

cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

## Windows

### Visual Studio generator

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release -j
ctest --test-dir build -C Release --output-on-failure

cmake --build build --config Debug -j
ctest --test-dir build -C Debug --output-on-failure
```

### Ninja from a Developer Command Prompt

```powershell
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
ctest --test-dir build/release --output-on-failure
```

Use a Developer PowerShell for Visual Studio so `cl.exe` and the Windows SDK are
on `PATH`. `MSVC` builds use `/W4 /permissive- /utf-8` and warnings are errors by
default.

## Sanitizers

AddressSanitizer and UndefinedBehaviorSanitizer are supported for GCC and Clang:

```bash
cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DNXRELINKER_ENABLE_SANITIZERS=ON
cmake --build build/asan -j"$(nproc)"
ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
ctest --test-dir build/asan --output-on-failure
```

The compiler's sanitizer runtime must be installed. For example, some Linux
distributions package Clang without `libclang_rt.asan-*`; install the matching
Clang runtime package or use GCC. MSVC sanitizers are not enabled by this option.

## Options

| Option | Default | Meaning |
| --- | --- | --- |
| `NXRELINKER_BUILD_TESTS` | `ON` | Build GoogleTest tests and CTest entries |
| `NXRELINKER_WARNINGS_AS_ERRORS` | `ON` | Treat first-party compiler warnings as errors |
| `NXRELINKER_ENABLE_SANITIZERS` | `OFF` | Add ASan/UBSan flags for GCC/Clang |

## Build output

```text
build/<preset>/nxinspect
build/<preset>/libnxrelinker.a
build/<preset>/tests/nxrelinker_tests
```

On Windows the static library and executable suffixes follow the MSVC toolchain
(`.lib`, `.exe`).

## Troubleshooting

- **GoogleTest download fails:** install GoogleTest system-wide or allow access
  to `github.com`/`codeload.github.com`. The FetchContent archive is pinned, so a
  later configure can reuse the CMake download cache.
- **Clang sanitizer link error:** install the Clang runtime package or configure
  the ASan build with GCC (`-DCMAKE_CXX_COMPILER=g++`).
- **Warnings are errors:** fix the warning. Do not set
  `NXRELINKER_WARNINGS_AS_ERRORS=OFF` in CI.
- **Ninja not found on Windows:** install Ninja and run from a Developer prompt,
  or use the Visual Studio generator.
- **Stale test discovery:** reconfigure or run `ctest` again; GoogleTest
  discovery runs in `PRE_TEST` mode.

## CI

`.github/workflows/ci.yml` builds Release and Debug on Linux and Windows, runs
CTest, and runs an ASan/UBSan job on Linux. The CI must remain green before
Phase 1 begins.
