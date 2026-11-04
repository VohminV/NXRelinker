# Security Policy

## Reporting a vulnerability

Report security issues privately to the maintainers rather than opening a public
issue with exploit details. Include the affected version or commit, a minimal
reproducer and the expected/observed behavior. Do not attach commercial game
files, keys, console dumps or save data.

## Supported versions

NXRelinker is in Phase 0 and has no stable release. Security fixes target the
`main` branch. Historical tags are not supported unless stated in a release
notice.

## Untrusted input model

`nxinspect` and future parsers treat every input file as untrusted. The parser
must be safe under:

- truncated and empty files;
- invalid signatures and conflicting signatures;
- integer overflow in offsets, sizes and address ranges;
- overlapping file or memory ranges;
- attacker-controlled segment counts and table offsets;
- very large files;
- non-UTF-8 paths and file names;
- compressed segments (currently reported but not decompressed).

Current guarantees:

- Reads are bounds-checked before access.
- 64-bit arithmetic is used for range checks.
- The parser does not execute analyzed code.
- `nxinspect` does not write to the input and only prints a report.
- The parser does not decrypt, decompress or unpack proprietary containers yet.

Known limitations:

- Decompression and relocation application are not implemented, so their
  future attack surface is not yet exercised.
- Fuzzing and a formal memory-safety audit are future work; the ASan/UBSan
  preset is the current automated safety net.
- JSON escaping handles quotes, backslashes, control characters and the standard
  short escapes. UTF-8 validation of paths is not performed.

## Handling user files

- Work on copies. Do not modify original game files or dumps.
- Do not commit or upload copyrighted binaries, keys, extracted assets or save
  data.
- Do not run untrusted analyzers, plugins or generated executables without an
  isolated environment.
- Do not use NXRelinker to bypass DRM, console authorization or content
  protection. Such functionality is explicitly out of scope.

## Dependency and supply-chain policy

- Dependencies require a documented reason, a license check and a reproducible
  version pin.
- FetchContent archives use a SHA-256 hash where the build fetches them.
- Third-party code is never patched silently; deviations are recorded.
- Do not introduce dependencies that require keys, console secrets or
  redistribution of proprietary SDK code.

## Disclosure

Once a fix is available, the maintainers may publish a short advisory with the
affected component, impact, fixed commit and mitigation. Credit is given unless
the reporter requests anonymity.
