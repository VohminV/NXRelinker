---
name: nvn-vulkan
description: Use for NVN command/state translation, Vulkan 1.3 backend, GPU memory, textures, tiling, render passes, shader IR and SPIR-V validation work.
---

# NVN to Vulkan

## Purpose

Translate NVN graphics behavior to a native Vulkan 1.3 backend while preserving
GPU state, resource layout and synchronization semantics. Shaders pass through
analysis and an explicit IR before SPIR-V generation. NVN is not OpenGL with a
different name.

## When to use

- Implementing NVN device, queue, command-buffer or memory-pool behavior.
- Translating textures, samplers, buffers, render targets or depth/stencil state.
- Implementing command translation, synchronization or descriptor binding.
- Adding shader analysis, IR or SPIR-V output.
- Setting up Vulkan Validation Layers or SPIR-V validation.
- Profiling a rendering path after correctness is established.

## Required sources

- `https://switchbrew.org/wiki/GPU`
- `https://registry.khronos.org/vulkan/`
- `https://registry.khronos.org/vulkan/specs/latest/html/vkspec.html`
- `https://docs.vulkan.org/`
- `https://registry.khronos.org/SPIR-V/`
- `https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html`
- `https://github.com/KhronosGroup/SPIRV-Cross`
- `https://github.com/KhronosGroup/SPIRV-Tools`
- `https://github.com/KhronosGroup/Vulkan-ValidationLayers`
- `https://git.ryujinx.app/ryujinx/ryujinx/src/branch/master/src/Ryujinx.Graphics.Shader`
  and `https://git.ryujinx.app/ryujinx/ShaderTools` (behavioral references only)
- `docs/research/REFERENCE_SOURCES.md`, `RESEARCH_LOG.md`

## Architecture

```text
NVN calls/state
      |
      v
NVN compatibility layer (no Vulkan types in the public NVN surface)
      |
      v
GPU state / command translation
      |
      v
resource and pipeline cache
      |
      v
Vulkan 1.3 backend
```

Shaders:

```text
NVN shader bytes -> decode -> shader IR -> SPIR-V -> pipeline
```

Keep the NVN-facing layer independent from Vulkan handles where practical. The
runtime must not contain hidden NVN policy, and the recompiler must not contain
graphics policy.

## Workflow

1. **Establish the contract.** Record the NVN state, command, texture format or
   shader encoding and its evidence. Unconfirmed behavior is `INVESTIGATING`.
2. **Create a synthetic test.** Use generated textures/buffers/shaders; no
   commercial assets.
3. **Implement one translation unit.** Keep state translation, resource
   management and command emission separate.
4. **Validate resources.** Check format compatibility, usage flags, image layout,
   tiling/swizzling assumptions and memory requirements.
5. **Validate synchronization.** Make barriers, render-pass dependencies,
   queue ownership and frame pacing explicit.
6. **Validate shaders.** Run SPIRV-Tools, then Vulkan Validation Layers.
7. **Test rendering.** Compare expected and actual output for a controlled scene.
8. **Measure** only after validation is clean.

## Texture and memory checklist

- [ ] Format mapping is documented and tested (color, depth, stencil, compressed).
- [ ] Tiling, swizzling and row alignment are verified, not guessed.
- [ ] Image/buffer usage and memory type requirements are checked.
- [ ] Descriptor binding and update-after-bind rules are explicit.
- [ ] Render-target and depth/stencil load/store semantics are correct.
- [ ] Aliasing and memory-pool reuse obey Vulkan lifetime rules.
- [ ] Synchronization covers transfer, render and presentation stages.
- [ ] Unsupported formats fail explicitly.

## Common mistakes

- Treating NVN as OpenGL and mapping states one-to-one.
- Using a fake Z-buffer or swapchain-only output to claim rendering success.
- Ignoring tiling/swizzling and sampling garbage.
- Creating Vulkan objects without checking limits or format support.
- Missing image-layout transitions or queue ownership transfers.
- Using a single global barrier as a substitute for correct synchronization.
- Emitting SPIR-V that was never validated.
- Copying an emulator's graphics architecture.
- Optimizing before the validation layers are clean.

## Checks

- [ ] NVN contract and source evidence recorded.
- [ ] Validation Layers clean for every new path.
- [ ] SPIRV-Tools validation clean for every generated module.
- [ ] Tests use synthetic assets and deterministic frames.
- [ ] Unsupported formats/features return explicit errors.
- [ ] Release/Debug builds and sanitizers pass.
- [ ] No hidden emulator or interpreter path.

## Completion criteria

- The tested scene renders with correct textures, depth and synchronization.
- Validation is clean.
- Limitations are documented.
- Performance work has before/after measurements.
- Public API or boundary changes have an ADR.

## Related tests

Future layout:

- `tests/graphics/` — resource creation, upload, pipeline and render-target tests.
- `tests/graphics/shaders/` — IR and SPIR-V validation.
