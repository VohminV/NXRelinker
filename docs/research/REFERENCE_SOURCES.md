# Reference Sources

This file is the mandatory source baseline for NXRelinker research. Every format,
runtime API, instruction encoding and graphics decision must be traceable to a
source listed here or to a newer primary source recorded in
[`RESEARCH_LOG.md`](RESEARCH_LOG.md).

## Verification policy

- Verification date: **2026-11-04**.
- Automated checks used HTTP GET with redirects enabled and a desktop user agent.
- `VERIFIED` means the canonical URL returned HTTP 200 on the check date. It does
  **not** mean every statement on the page is correct or current.
- `REDIRECTED` means the original URL responded and the final URL is recorded.
- `RESTRICTED` means the site blocked automated access (HTTP 403, 451 or similar);
  the URL may still work in a browser.
- `UNAVAILABLE` means the canonical URL returned HTTP 404. The original URL is
  intentionally retained; a verified replacement is listed instead of silently
  rewriting history.
- `UNREACHABLE` means DNS, TLS or network failure at check time.
- Links are pointers, not permission to copy. Check every dependency and
  documentation license before incorporating code, prose or data.

> Legal scope: NXRelinker does not include DRM circumvention, console
> cryptographic keys, decrypted commercial content or redistribution of
> copyrighted game assets. Community links that discuss such topics are listed
> only as historical research pointers. Local law and the terms of each source
> apply.

## 8.1 Nintendo Switch formats

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| FMT-01 | Switchbrew — File formats | https://switchbrew.org/wiki/File_formats | Historical index of container and executable formats | **UNAVAILABLE (404)** | Original retained. Verified replacements: FMT-02, FMT-03, FMT-04 for NCA/NSO/NRO; no verified general file-format index replacement was found. |
| FMT-02 | Switchbrew — NCA | https://switchbrew.org/wiki/NCA | NCA, PFS0 and RomFS sections; content filesystem context | **VERIFIED (200)** | Community wiki; no explicit reuse license observed on page. Reference only. |
| FMT-03 | Switchbrew — NRO | https://switchbrew.org/wiki/NRO | NRO/Rocrt header, flags, AdditionalNroHeader, assets | **VERIFIED (200)** | Community wiki; latest revision 14197 (2025-12-19); reference only. |
| FMT-04 | Switchbrew — NSO | https://switchbrew.org/wiki/NSO | NSO header, flags, compression, dynamic tables, hashes | **VERIFIED (200)** | Community wiki; latest revision 14193 (2025-12-19); reference only. |
| FMT-05 | Switchbrew — PFS0 | https://switchbrew.org/wiki/PFS0 | PartitionFS container layout | **UNAVAILABLE (404)** | Original retained. URL is 404; verified alternative context is the PFS0 section of FMT-02. |
| FMT-06 | Switchbrew — RomFs | https://switchbrew.org/wiki/RomFs | RomFS directory and file table layout | **UNAVAILABLE (404)** | Original retained. URL is 404; verified alternative context is the RomFS sections of FMT-02. |
| FMT-07 | Switchbrew — BNTX | https://switchbrew.org/wiki/BNTX | BNTX texture container | **UNAVAILABLE (404)** | Original retained. No wiki replacement found; use implementation references FMT-20, FMT-21 and validate independently. |
| FMT-08 | Switchbrew — BFRES | https://switchbrew.org/wiki/BFRES | BFRES model/resource container | **UNAVAILABLE (404)** | Original retained. No wiki replacement found; use FMT-15, FMT-16 and validate independently. |
| FMT-09 | Switchbrew — AAMP | https://switchbrew.org/wiki/AAMP | AAMP parameter archive | **UNAVAILABLE (404)** | Original retained. No wiki replacement found; use FMT-17 and validate independently. |
| FMT-10 | Switchbrew — SARC | https://switchbrew.org/wiki/SARC | SARC archive container | **UNAVAILABLE (404)** | Original retained. No wiki replacement found; use FMT-20A and FMT-20B and validate independently. |

## 8.2 Extraction and conversion tools

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| FMT-11 | LibHac | https://github.com/Thealexbarney/LibHac | Historical C# reference for Switch formats and NCA handling | **UNAVAILABLE (404)** | Repository removed from GitHub. Original URL retained. No canonical replacement verified; see FMT-30, FMT-31 and the research log. |
| FMT-12 | nsz | https://github.com/nicoboss/nsz | NSP/XCI/NSZ compression implementation and container handling | **VERIFIED (200)** | GitHub reports `NOASSERTION`; inspect `LICENSE` before reuse. |
| FMT-13 | nut | https://github.com/blawar/nut | USB/network install tool; PFS0/NSP handling reference | **VERIFIED (200)** | GPL-3.0. |
| FMT-14 | NXGameInfo | https://github.com/ArchLeaders/NXGameInfo | Title metadata inspection | **UNAVAILABLE (404)** | Repository removed. Original retained; no verified replacement found. |
| FMT-15 | Switch-Toolbox | https://github.com/KillzXGaming/Switch-Toolbox | BFRES/BNTX/SARC/AAMP tooling and format research | **VERIFIED (200)** | GPL-3.0. Reference only unless the license is compatible with the integration. |
| FMT-16 | BfresLibrary | https://github.com/KillzXGaming/BfresLibrary | BFRES parser/writer | **VERIFIED (200)** | MIT. |
| FMT-17 | AampLibrary | https://github.com/KillzXGaming/AampLibrary | AAMP parser/writer | **VERIFIED (200)** | No SPDX license detected; verify before reuse. |
| FMT-18 | SarcLib (KillzXGaming) | https://github.com/KillzXGaming/SarcLib | SARC parser/writer | **UNAVAILABLE (404)** | Original retained. Verified alternatives: FMT-20A (MIT) and FMT-20B (GPL-3.0). |
| FMT-19 | Nintendo.NX.SARC | https://github.com/KillzXGaming/Nintendo.NX.SARC | SARC handling | **UNAVAILABLE (404)** | Original retained. Verified alternative: FMT-20A. |
| FMT-20 | BNTX-Extractor | https://github.com/aboood40091/BNTX-Extractor | BNTX texture extraction | **VERIFIED (200)** | GPL-3.0. |
| FMT-20A | SarcLib (ThemezerNX) | https://github.com/ThemezerNX/SarcLib | SARC/SZS parser and packer | **VERIFIED (200)** | MIT. |
| FMT-20B | SarcLib (aboood40091) | https://github.com/aboood40091/SarcLib | SARC parser | **VERIFIED (200)** | GPL-3.0. |
| FMT-21 | GTX-Extractor | https://github.com/aboood40091/GTX-Extractor | Wii U GTX texture extraction, related texture-layout research | **VERIFIED (200)** | GPL-3.0. |
| FMT-22 | switch-tools | https://github.com/switchbrew/switch-tools | Reference implementations of `elf2nso` and `elf2nro` | **VERIFIED (200)** | ISC-family homebrew tooling; verify per-file headers. |
| FMT-23 | libnx | https://github.com/switchbrew/libnx | Homebrew SDK structures for NRO/NACP and Horizon APIs | **VERIFIED (200)** | ISC. |
| FMT-24 | System V gABI — ELF header | https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html | ELF header field names, offsets and constraints | **VERIFIED (200)** | Generic ABI specification; reference. |
| FMT-25 | Linux man-pages — elf(5) | https://man7.org/linux/man-pages/man5/elf.5.html | Cross-check for ELF64 field layout and values | **VERIFIED (200)** | Linux man-pages license; reference only. |

## 8.3 Shaders and graphics

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| GFX-01 | Switchbrew — GPU | https://switchbrew.org/wiki/GPU | Tegra X1 GPU, NVN-related documentation | **VERIFIED (200)** | Community wiki; reference only. |
| GFX-02 | Ryujinx shader engine | https://github.com/Ryujinx/Ryujinx/tree/master/src/Ryujinx.Graphics.Shader | ARM64 GPU shader translation reference | **UNAVAILABLE (404)** | Original GitHub org removed. Verified replacement: GFX-02A. |
| GFX-02A | Ryujinx shader engine (official Forgejo) | https://git.ryujinx.app/ryujinx/ryujinx/src/branch/master/src/Ryujinx.Graphics.Shader | Shader translation reference | **VERIFIED (200)** | MIT; reference only, do not copy architecture wholesale. |
| GFX-03 | ShaderTools | https://github.com/ryujinx-graphics/ShaderTools | Shader IR and translation experiments | **UNAVAILABLE (404)** | Original retained. Verified replacement: GFX-03A. |
| GFX-03A | ShaderTools (official Forgejo) | https://git.ryujinx.app/ryujinx/ShaderTools | Shader IR and translation experiments | **VERIFIED (200)** | Verify repository license before reuse. |
| GFX-04 | SPIRV-Cross | https://github.com/KhronosGroup/SPIRV-Cross | SPIR-V reflection and cross-compilation | **VERIFIED (200)** | Apache-2.0. |
| GFX-05 | yuzu shader recompiler mirror | https://github.com/yuzu-mirror/shader_recompiler | Historical shader recompiler reference | **UNAVAILABLE (404)** | Original retained. No verified canonical mirror found; do not substitute an unverified fork. |
| GFX-06 | godot-nvn-shader-converter | https://github.com/godotengine/godot-nvn-shader-converter | Historical NVN shader conversion experiment | **UNAVAILABLE (404)** | Original retained. No verified replacement found. |
| GFX-07 | Vulkan specification | https://registry.khronos.org/vulkan/specs/latest/html/vkspec.html | Authoritative Vulkan behavior | **VERIFIED (200)** | Khronos specification copyright; cite, do not copy wholesale. |
| GFX-08 | SPIR-V specification | https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html | Authoritative SPIR-V semantics | **VERIFIED (200)** | Khronos specification copyright; cite. |

## 8.4 ARM64 reverse engineering

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| RE-01 | Ghidra-Switch-Loader | https://github.com/Adubbz/Ghidra-Switch-Loader | Ghidra loader for Switch executables | **VERIFIED (200)** | ISC. |
| RE-02 | ida-switch-loader | https://github.com/roblabla/ida-switch-loader | Historical IDA loader | **UNAVAILABLE (404)** | Original retained. Verified alternative: RE-02A. |
| RE-02A | SwitchIDAProLoader | https://github.com/pgarba/SwitchIDAProLoader | IDA Pro loader for NRO binaries | **VERIFIED (200)** | No SPDX license detected; verify before reuse. |
| RE-03 | Ghidra | https://ghidra-sre.org/ | SRE framework and ARM64 disassembly/analysis | **REDIRECTED (200 → GitHub)** | Apache-2.0 (official repository). |
| RE-04 | Dogbolt | https://dogbolt.org/ | Multi-decompiler comparison service | **VERIFIED (200)** | Web service terms; do not upload sensitive binaries. |
| RE-05 | capa | https://github.com/mandiant/capa | Capability detection for analyzed binaries | **VERIFIED (200)** | Apache-2.0. |
| RE-06 | Ghidra-Horizon-Kernel | https://github.com/Atmosphere-NX/Ghidra-Horizon-Kernel | Historical Horizon kernel analysis helpers | **UNAVAILABLE (404)** | Original retained. Verified alternatives: RE-01 and RE-06A. |
| RE-06A | Atmosphère | https://github.com/Atmosphere-NX/Atmosphere | Horizon OS behavior and service research | **VERIFIED (200)** | GPL-2.0. Reference only. |

## 8.5 Runtime and emulator references

These projects are behavioral references only. NXRelinker does not embed or
redistribute emulator code and does not copy their architecture.

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| RT-01 | Ryujinx | https://github.com/Ryujinx/Ryujinx | Emulator behavior, service and GPU reference | **UNAVAILABLE (404)** | Original GitHub org removed. Verified replacement: RT-01A. |
| RT-01A | Ryujinx (official Forgejo) | https://git.ryujinx.app/ryujinx/ryujinx | Emulator behavior, service and GPU reference | **VERIFIED (200)** | MIT; reference only. |
| RT-02 | sudachi | https://github.com/sudachi-emu/sudachi | Historical emulator behavior reference | **UNAVAILABLE (404)** | Original retained. No verified canonical replacement recorded; do not use unverified forks as authority. |
| RT-03 | torzu | https://github.com/torzu-emulator/torzu | Historical emulator behavior reference | **UNAVAILABLE (404)** | Original retained. `git.torzu.org` did not resolve from the check host. |
| RT-04 | yuzu mirror | https://github.com/yuzu-mirror/yuzu | Historical emulator behavior reference | **RESTRICTED (451)** | DMCA-restricted. Do not mirror or redistribute. Retained as historical pointer only. |

## 8.6 Engines and game resources

These sources are for container and asset research. They are not Nintendo
Switch-specific authorities and must be validated before use.

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| AST-01 | AssetRipper | https://github.com/AssetRipper/AssetRipper | Unity asset extraction architecture reference | **VERIFIED (200)** | GPL-3.0. |
| AST-02 | UnityCsReference | https://github.com/Unity-Technologies/UnityCsReference | Unity runtime reference source | **VERIFIED (200)** | Unity Companion License (`NOASSERTION`); proprietary terms, reference only. |
| AST-03 | UAssetAPI (requested URL) | https://github.com/UAssetAPI/UAssetAPI | Unreal asset parsing reference | **UNAVAILABLE (404)** | Original retained. Verified alternative: AST-03A. |
| AST-03A | UAssetAPI (atenfyr) | https://github.com/atenfyr/UAssetAPI | Unreal asset parsing library | **VERIFIED (200)** | MIT. |
| AST-04 | WolvenKit | https://github.com/WolvenKit/WolvenKit | REDengine asset tooling reference | **VERIFIED (200)** | GPL-3.0. |
| AST-05 | NifSkope | https://github.com/niftools/nifskope | NIF model format tooling reference | **VERIFIED (200)** | `NOASSERTION`; verify license before reuse. |

## 8.7 Communities and research databases

Community sources are useful for observations and test cases. They are **not**
sufficient as the sole confirmation of a technical fact.

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| COM-01 | GBAtemp Switch modding | https://gbatemp.net/forums/nintendo-switch-modding.284/ | Community observations | **RESTRICTED (403; browser required)** | Forum terms apply; content authorship varies. |
| COM-02 | r/SwitchMods | https://www.reddit.com/r/SwitchMods/ | Community observations | **RESTRICTED (403; browser required)** | Reddit terms apply. |
| COM-03 | SwitchMods Discord | https://discord.gg/switchmods | Community chat | **UNREACHABLE (TLS handshake timeout)** | Invite may be expired or blocked; no technical authority. |
| COM-04 | GameBanana Switch | https://gamebanana.com/games/5617 | Community mod research | **VERIFIED (200)** | Site terms and per-item licenses apply. |
| COM-05 | Nexus Mods Switch | https://nexusmods.com/nintendoswitch | Community mod research | **RESTRICTED (403 after redirect; browser required)** | Site terms and per-item licenses apply. |

## 9. Additional mandatory technical sources

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| STD-01 | ARM documentation | https://developer.arm.com/documentation/ | Architecture and instruction set manuals | **REDIRECTED (200 → support.arm.com)** | Arm documentation terms; cite. |
| STD-02 | Arm A-Profile Architecture | https://developer.arm.com/Architectures/A-Profile%20Architecture | AArch64 architecture overview | **VERIFIED (200)** | Arm terms; cite. |
| STD-03 | LLVM documentation | https://llvm.org/docs/ | Compiler, MC and JIT documentation | **VERIFIED (200)** | Apache-2.0 WITH LLVM-exception. |
| STD-04 | LLVM ORCv2 | https://llvm.org/docs/ORCv2.html | JIT infrastructure reference | **VERIFIED (200)** | Apache-2.0 WITH LLVM-exception. |
| STD-05 | LLVM code generator | https://llvm.org/docs/CodeGenerator.html | Instruction selection and lowering reference | **VERIFIED (200)** | Apache-2.0 WITH LLVM-exception. |
| STD-06 | LLVM project | https://github.com/llvm/llvm-project | LLVM MC, disassembler, ORC and codegen source | **VERIFIED (200)** | Apache-2.0 WITH LLVM-exception. |
| STD-07 | Vulkan registry | https://registry.khronos.org/vulkan/ | Vulkan headers, specs and extensions | **VERIFIED (200)** | Khronos terms. |
| STD-08 | Vulkan documentation | https://docs.vulkan.org/ | Vulkan guides and tutorials | **VERIFIED (200)** | Khronos terms. |
| STD-09 | Vulkan Validation Layers | https://github.com/KhronosGroup/Vulkan-ValidationLayers | Runtime validation of Vulkan usage | **VERIFIED (200)** | Apache-2.0. |
| STD-10 | SPIR-V registry | https://registry.khronos.org/SPIR-V/ | SPIR-V specifications and grammars | **VERIFIED (200)** | Khronos terms. |
| STD-11 | SPIRV-Tools | https://github.com/KhronosGroup/SPIRV-Tools | SPIR-V validation and optimization | **VERIFIED (200)** | Apache-2.0. |
| STD-12 | NVIDIA Tegra embedded hardware | https://developer.nvidia.com/embedded/develop/hardware | Tegra platform documentation index | **REDIRECTED (200 → Jetson modules)** | NVIDIA documentation terms; original URL retained. |
| STD-13 | CMake documentation | https://cmake.org/cmake/help/latest/ | Build system reference | **VERIFIED (200)** | BSD-3-Clause. |
| STD-14 | Ghidra | https://github.com/NationalSecurityAgency/ghidra | SRE framework and ARM64 analysis | **VERIFIED (200)** | Apache-2.0. |

## 9A. Toolchain and standards added by this repository

| ID | Source | URL | Purpose | Status on 2026-11-04 | License / caveat |
| --- | --- | --- | --- | --- | --- |
| STD-15 | GoogleTest | https://github.com/google/googletest | Unit and differential test framework | **VERIFIED (200)** | BSD-3-Clause. Pinned as a FetchContent archive with SHA-256 in `cmake/FetchGoogleTest.cmake`. |
| STD-16 | Ninja | https://ninja-build.org/ | Build executor | **VERIFIED (200)** | Apache-2.0. |
| STD-17 | OpenCode configuration schema | https://opencode.ai/config.json | Validate `.opencode/opencode.json` | **VERIFIED (200)** | Configuration schema; no code reuse. |

## Source gaps and follow-up requirements

1. `File_formats`, `PFS0`, `RomFs`, `BNTX`, `BFRES`, `AAMP` and `SARC` wiki pages
   returned 404 on 2026-11-04. Do not claim their contents as verified until a
   primary source or archived revision is independently checked. The original
   URLs stay in this file.
2. The canonical LibHac repository is unavailable. No verified canonical mirror
   was found. Its conclusions must be re-derived from Switchbrew, libnx, switch-tools
   and targeted tests.
3. Asset-format pages without documentation must be researched from the
   implementation projects above, and every resulting offset or field must be
   recorded as a testable hypothesis in `RESEARCH_LOG.md` before it becomes a
   production parser.
4. Emulator repositories are behavioral references only. Do not copy their
   architecture, and do not use them as a substitute for Horizon OS or NVN
   specifications that still require independent verification.
