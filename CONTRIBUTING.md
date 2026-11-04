# Contributing to NXRelinker

## Scope

Contributions must stay within first-generation Nintendo Switch:
Tegra X1, Cortex-A57/AArch64, GM20B Maxwell, Horizon OS, NVN and the
NSO/NRO/ELF64 executable family. Do not add PlayStation, Xbox, Tegra X2, T239 or
Nintendo Switch 2 assumptions.

NXRelinker is not an emulator. Emulator repositories are behavioral references
only. Do not copy emulator code, architecture or assets into this repository.

## Before you start

1. Read [`AGENTS.md`](AGENTS.md).
2. Read the relevant skill under `.opencode/skills/`.
3. Check [`docs/research/REFERENCE_SOURCES.md`](docs/research/REFERENCE_SOURCES.md).
4. If the change affects a public API or crosses a subsystem boundary, record an
   ADR in [`docs/architecture/DECISIONS.md`](docs/architecture/DECISIONS.md)
   first.
5. Do not use commercial game files, console keys, save data or extracted
   assets in tests. Use synthetic builders under `tests/binary/` or generated
   fixtures.

## Development workflow

```bash
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j"$(nproc)"
ctest --test-dir build/debug --output-on-failure

cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j"$(nproc)"
ctest --test-dir build/release --output-on-failure
```

Sanitizer build:

```bash
cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DNXRELINKER_ENABLE_SANITIZERS=ON
cmake --build build/asan -j"$(nproc)"
ASAN_OPTIONS=detect_leaks=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
ctest --test-dir build/asan --output-on-failure
```

Formatting and static analysis use `.clang-format` and `.clang-tidy` when those
tools are available. Do not reformat unrelated code.

## Code rules

- C++20, standard library first.
- Warnings are errors for first-party targets. Do not disable warnings to pass.
- Parsers are bounds-checked, deterministic and safe on untrusted input.
- No fake implementations or placeholder success paths in advertised features.
- No comments that restate the code. Explain non-obvious format or semantics
  decisions and cite the source.
- Keep subsystem boundaries: parsers do not know about runtime policy; runtime
  does not parse executable headers.
- Add or update tests with every behavioral change.

## Research rules

- Register every new source in `docs/research/REFERENCE_SOURCES.md`.
- Record findings in `docs/research/RESEARCH_LOG.md` using its template and
  status values.
- Mark unconfirmed behavior as `UNVERIFIED` or `INVESTIGATING`.
- If a link is dead, retain it and add a verified alternative and the check date.
- Community posts, decompiler output and emulator behavior are supporting
  evidence, not primary authority.

## Pull-request checklist

- [ ] The change is inside the Switch gen-1 scope.
- [ ] Release and Debug builds pass.
- [ ] Tests were added or updated and pass.
- [ ] Sanitizer build passes when the change touches parsing or memory.
- [ ] Sources and research log are updated.
- [ ] No keys, commercial binaries, extracted assets or save files are included.
- [ ] Limitations and unsupported paths are documented honestly.
- [ ] The task report in `AGENTS.md` format is included.

## Commit hygiene

Keep commits focused. Do not commit build output (`build/`, `out/`), editor
state, logs or local benchmark results. Never commit secrets. If a hook rejects
a commit, fix the cause and create a new commit instead of bypassing the hook.

## Licensing

The project is MIT-licensed. New source files are accepted under the project
license unless explicitly documented otherwise. Third-party dependencies must
have a compatible license, a documented reason and a reproducible pin. Reference
projects under incompatible licenses may be studied but not copied.
