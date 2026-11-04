# Testing

## Running tests

```bash
cmake --build build/debug -j"$(nproc)"
ctest --test-dir build/debug --output-on-failure
```

`ctest -N` lists tests. GoogleTest cases are discovered per test case, so a
failure identifies the parser behavior directly.

## Test layout

```text
tests/binary/                 executable-format tests
tests/binary/synthetic_builders.h  synthetic NSO/NRO/ELF builders
tests/unit/                   subsystem-neutral tests
tests/fixtures/synthetic/     generated or purely synthetic fixture home
```

No test may require a console, cryptographic keys, commercial game files,
extracted assets or a specific emulator.

## Current coverage

### Format detection

- NSO at offset 0.
- NRO at offset 0x10.
- Rejection of `NRO0` at offset 0.
- ELF at offset 0.
- Empty and short inputs.

### NSO

- Valid header fields, module ID, BSS and segments.
- Compressed segment metadata.
- Zero compressed size for a compressed segment.
- Truncated header and invalid magic (fatal).
- Segment file range outside the input.
- Overlapping file ranges.
- Unknown flag bits.
- BSS address-space overflow.
- Module name outside the file.

### NRO

- Valid Rocrt/NRO header fields.
- Magic at the wrong offset and truncated input (fatal).
- Declared size beyond the file.
- Trailing asset bytes.
- Compressed-image warning.
- Overlapping memory segments.
- Invalid additional `NRA0` header.

### ELF64

- Valid AArch64 header, including `EI_VERSION`.
- ELF32 and big-endian rejection.
- Unknown identification version warning.
- Non-AArch64 machine warning.
- Program-header table outside the file.
- Truncated header.

### Unit utilities

- Checked range math, overlap detection and saturating addition.
- Bounds-checked little-endian reads and magic comparison.
- Severity counts and offsets.
- JSON string escaping and nested object/array generation.
- Hex encoding and text/JSON report content.
- JSON path escaping, segment nulls and unknown-format errors.

### CLI smoke tests

- `nxinspect --version`
- `nxinspect --help`
- Invalid option exits non-zero.

## Adding a test

1. Add a synthetic builder or extend the existing one in
   `tests/binary/synthetic_builders.h`. Never add commercial data.
2. Add the test to an existing executable-format test file or create a new one
   and register it in `tests/CMakeLists.txt`.
3. Test valid, truncated, invalid-signature and boundary cases.
4. Use diagnostics codes rather than message text for assertions.
5. Run Debug, Release and ASan/UBSan before submitting.

## Future test layers

- Differential ARM64 vs x86-64 tests for each supported instruction family.
- Relocation and symbol-resolution tests on synthetic modules.
- Horizon IPC serialization and service error-path tests.
- Vulkan Validation Layers and SPIR-V validation for graphics.
- Fuzz targets for every parser boundary; corpus files must be synthetic.

## Failure policy

Do not suppress or skip a failing test to make CI green. Fix the root cause or
record the limitation and keep the test failing/disabled with an explicit reason
in the task report. Do not report a test run that was not executed.
