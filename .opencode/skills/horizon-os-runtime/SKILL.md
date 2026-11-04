---
name: horizon-os-runtime
description: Use for Horizon OS IPC, services, handles, threads, memory, synchronization, FS, HID, time, audio and applet compatibility work in NXRelinker.
---

# Horizon OS Runtime

## Purpose

Implement a minimal, testable Horizon OS compatibility runtime that lets
relinked/recompiled applications execute without emulating the Tegra X1. Start
with the smallest service surface needed by a synthetic test application and
grow only against verified contracts.

## When to use

- Designing or implementing IPC request/response serialization.
- Adding a service (SM, FS, HID, VI, time, audio, applet).
- Implementing handles, threads, synchronization, shared memory or virtual memory.
- Adding runtime error semantics or tests.
- Debugging a recompiled application's service call.

## Required sources

- `https://switchbrew.org/wiki/NCA` for content context
- `https://switchbrew.org/wiki/ExeFS`
- `https://switchbrew.org/wiki/NACP_Format`
- `https://github.com/Atmosphere-NX/Atmosphere` for Horizon behavior research
  (GPL-2.0; reference only, do not copy)
- `https://git.ryujinx.app/ryujinx/ryujinx` for behavioral evidence
  (MIT; reference only)
- `https://github.com/switchbrew/libnx` for homebrew-facing service declarations
- `docs/research/REFERENCE_SOURCES.md`, `RESEARCH_LOG.md`

The standalone `PFS0` and `RomFs` wiki pages are 404. Record alternatives rather
than inventing layouts.

## Runtime boundaries

- `runtime`/`horizon` own service semantics, handles and IPC.
- They must not parse NSO/NRO headers; use the linker/binary APIs.
- They must not contain recompiler logic or NVN state.
- They must not emulate CPU instructions.
- Unimplemented commands return an explicit Horizon-style error, never success.

## Workflow

1. **Select the smallest surface.** Identify the exact service command needed by
   the test application and its dependencies.
2. **Document the contract.** Request/response layout, handle types, buffer
   descriptors, alignment, copy semantics, error codes and unsupported flags.
3. **Record evidence.** Add a research entry with source URL and status.
4. **Implement serialization first.** Build a pure function with a round-trip
   test before wiring dispatch.
5. **Implement semantics.** Keep state in a service object with explicit
   lifetime and thread-safety rules.
6. **Test error paths.** Invalid handle, invalid command, short buffer, wrong
   alignment, closed service and concurrent access.
7. **Validate with a synthetic app.** No commercial game or console required.
8. **Document status.** Mark each command as implemented, partial or unsupported.

## Service design checklist

- [ ] Command ID and request/response sizes documented.
- [ ] Buffer descriptor types and copy directions documented.
- [ ] Handle creation, duplication, closing and invalidation tested.
- [ ] Error codes match the verified contract or are marked provisional.
- [ ] Thread safety and reentrancy rules stated.
- [ ] Shared-memory and virtual-memory ownership is explicit.
- [ ] Unsupported commands return an error.
- [ ] Tests cover success, malformed input and lifetime errors.

## Common mistakes

- Inventing a command layout from an emulator without independent evidence.
- Returning success for unimplemented functionality.
- Treating a handle as a raw integer without lifetime/validation rules.
- Ignoring copy semantics or alignment for IPC buffers.
- Mixing service policy into the recompiler or binary parser.
- Assuming all services are available on all applet modes.
- Using a commercial game as the first integration test.
- Implementing threading with undefined synchronization behavior.
- Claiming audio/VI support before the core IPC path is tested.

## Checks

- [ ] Every implemented service has input/output/error documentation.
- [ ] Every command has serialization and malformed-input tests.
- [ ] Handle lifetime and double-close behavior are tested.
- [ ] Unsupported paths fail explicitly.
- [ ] Runtime does not parse executable formats.
- [ ] Release, Debug and ASan/UBSan tests pass.
- [ ] No keys, DRM bypass or console authorization code.

## Completion criteria

- The documented service subset is implemented and tested.
- A synthetic test application reaches the new path.
- Limitations and provisional assumptions are listed.
- No emulator or copyrighted code is embedded.
- IPC and handle behavior are deterministic and thread-safe by contract.

## Related tests

Future layout:

- `tests/runtime/` — handles, memory, synchronization, error paths.
- `tests/integration/` — synthetic application/service flows.
