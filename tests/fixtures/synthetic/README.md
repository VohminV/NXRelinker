# Synthetic fixtures

Fixtures in this directory must be generated from public format descriptions or
created by `examples/synthetic/make_synthetic.py`. They must never contain data
extracted from a commercial game, console key material or user save data.

The current unit tests build their inputs in memory through
`tests/binary/synthetic_builders.h`; no binary fixture files are required for
Phase 0. Generated files may be written to a temporary directory for manual CLI
testing and must remain untracked (`*.nso`, `*.nro` are ignored by `.gitignore`).

To create a manual fixture:

```bash
python3 examples/synthetic/make_synthetic.py --out /tmp/nxrelinker-fixtures
./build/release/nxinspect --format json /tmp/nxrelinker-fixtures/synthetic.nso
```
