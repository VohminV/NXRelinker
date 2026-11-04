---
name: regression-testing
description: Use for CTest/GoogleTest changes, synthetic fixtures, regression triage, sanitizer runs, CI validation, differential tests and benchmark regression checks.
---

# Regression Testing

## Purpose

Prevent behavior regressions and make verification reproducible on Linux and
Windows. Tests are evidence, not decoration, and every behavioral change needs
one.

## When to use

- Adding or modifying tests or CTest registration.
- Fixing a bug that needs a regression test.
- Changing a parser, diagnostic or JSON schema.
- Updating CI or build presets.
- Triaging a failing or flaky test.
- Adding differential or graphics validation.

## Required sources and commands

- `AGENTS.md` — definition of done.
- `docs/development/TESTING.md` — test layout and coverage.
- `docs/development/BUILDING.md` — build matrix.
- `cmake/FetchGoogleTest.cmake` — pinned test dependency.
- `.github/workflows/ci.yml` — CI jobs.
- Commands:

```bash
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j"$(nproc)"
ctest --test-dir build/debug --output-on-failure

cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j"$(nproc)"
ctest --test-dir build/release --output-on-failure
```

## Test design rules

- Test observable behavior and stable diagnostic codes, not incidental wording.
- Use synthetic in-memory inputs. Never require a console, keys or commercial
  data.
- Cover valid, truncated, invalid-magic, out-of-range, overlap and boundary
  cases for parsers.
- Cover both fatal and recoverable parse paths.
- Keep tests deterministic; no sleeps, wall-clock thresholds or network access.
- Name tests after the behavior, for example `DetectsOverlappingFileRanges`.
- One logical assertion per test where practical; use parameterized tests for
  encoding tables.
- Prefer the smallest fixture that exercises the bug.
- Keep test parallelism safe. Current tests are independent.

## Adding a test

1. Reproduce the behavior or the bug.
2. Add a test that fails for the right reason.
3. Implement the fix, not a test workaround.
4. Extend the synthetic builder if needed.
5. Register new test files in `tests/CMakeLists.txt`.
6. Run Debug, Release and sanitizer suites.
7. Update coverage documentation if the test category is new.

## Regression triage

1. Read the failure and reproduce locally.
2. Determine whether the test or the production code is wrong.
3. Check recent changes in the affected subsystem.
4. If the test is wrong, explain why and fix the test.
5. If the code is wrong, fix the root cause and keep the test.
6. If the failure is environmental, record it and do not hide it.
7. Never loop retries to mask a flake; find the nondeterminism.

## Sanitizers

```bash
cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DNXRELINKER_ENABLE_SANITIZERS=ON
cmake --build build/asan -j"$(nproc)"
ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
ctest --test-dir build/asan --output-on-failure
```

Sanitizer findings are bugs, not warnings. Do not suppress them without an ADR
and a documented justification.

## CI rules

- CI must run Release and Debug on Linux and Windows.
- The sanitizer job runs on Linux with GCC or Clang.
- Do not add `continue-on-error` to hide failures.
- Do not cache build directories in a way that masks reconfigure bugs.
- Network access is allowed for the pinned GoogleTest fetch; do not add
  unpinned downloads.
- Keep CI commands identical to documented local commands where possible.

## Common mistakes

- Testing only the happy path.
- Asserting on diagnostic prose instead of the code.
- Using a real game file because it is convenient.
- Marking a flaky test skipped without a reason.
- Changing the test to match a bug.
- Suppressing ASan/UBSan output.
- Forgetting to register a new test file.
- Treating a green build as proof of correctness.
- Reporting a test result from a previous run.

## Checks

- [ ] New behavior has a test.
- [ ] Negative paths are covered.
- [ ] Test is deterministic and isolated.
- [ ] Test is registered and discovered by CTest.
- [ ] Release, Debug and sanitizer runs pass.
- [ ] CI configuration matches local commands.
- [ ] No proprietary inputs are required.
- [ ] The report states exactly which commands ran.

## Completion criteria

- The regression is covered by a test that fails without the fix.
- All suites pass on the supported local configurations.
- CI changes are validated or explicitly marked as pending external CI.
- No failing test is hidden or silently skipped.

## Related tests

- `tests/binary/test_format_detection.cpp`
- `tests/binary/test_nso.cpp`
- `tests/binary/test_nro.cpp`
- `tests/binary/test_elf.cpp`
- `tests/binary/test_inspection.cpp`
- CLI smoke tests in `tests/CMakeLists.txt`
