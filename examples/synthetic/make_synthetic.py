#!/usr/bin/env python3
"""Create synthetic NSO and NRO files for manual nxinspect demonstrations.

The output is not derived from any commercial binary. It is a minimal byte
fixture that follows the documented header layouts.
"""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


def put_u32(buffer: bytearray, offset: int, value: int) -> None:
    struct.pack_into("<I", buffer, offset, value)


def build_nso() -> bytes:
    buffer = bytearray(0x180)
    buffer[0:4] = b"NSO0"
    put_u32(buffer, 0x04, 0)
    put_u32(buffer, 0x08, 0)
    put_u32(buffer, 0x0C, 0)

    # text: file 0x100, memory 0x0, 0x40 bytes
    put_u32(buffer, 0x10, 0x100)
    put_u32(buffer, 0x14, 0x0000)
    put_u32(buffer, 0x18, 0x40)
    # ro: file 0x140, memory 0x1000, 0x20 bytes
    put_u32(buffer, 0x20, 0x140)
    put_u32(buffer, 0x24, 0x1000)
    put_u32(buffer, 0x28, 0x20)
    # data: file 0x160, memory 0x2000, 0x20 bytes
    put_u32(buffer, 0x30, 0x160)
    put_u32(buffer, 0x34, 0x2000)
    put_u32(buffer, 0x38, 0x20)

    put_u32(buffer, 0x1C, 0x100)
    put_u32(buffer, 0x2C, 0)
    put_u32(buffer, 0x3C, 0x10)
    buffer[0x40:0x60] = bytes(range(32))
    return bytes(buffer)


def build_nro() -> bytes:
    buffer = bytearray(0x200)
    put_u32(buffer, 0x00, 0x8)
    buffer[0x10:0x14] = b"NRO0"
    put_u32(buffer, 0x14, 0)
    put_u32(buffer, 0x18, len(buffer))
    put_u32(buffer, 0x1C, 0)

    put_u32(buffer, 0x20, 0x0000)
    put_u32(buffer, 0x24, 0x40)
    put_u32(buffer, 0x28, 0x40)
    put_u32(buffer, 0x2C, 0x40)
    put_u32(buffer, 0x30, 0x80)
    put_u32(buffer, 0x34, 0x40)
    put_u32(buffer, 0x38, 0x10)
    buffer[0x40:0x60] = bytes(range(32))
    return bytes(buffer)


def build_elf64() -> bytes:
    program_header_offset = 0x40
    program_header_size = 56
    buffer = bytearray(program_header_offset + program_header_size)
    buffer[0:4] = b"\x7fELF"
    buffer[4] = 2  # ELFCLASS64
    buffer[5] = 1  # ELFDATA2LSB
    buffer[6] = 1  # EV_CURRENT
    struct.pack_into("<HHI", buffer, 0x10, 2, 183, 1)  # ET_EXEC, EM_AARCH64
    struct.pack_into("<Q", buffer, 0x18, 0x400000)
    struct.pack_into("<Q", buffer, 0x20, program_header_offset)
    struct.pack_into("<Q", buffer, 0x28, 0)
    struct.pack_into("<I", buffer, 0x30, 0)
    struct.pack_into("<HHHHHH", buffer, 0x34, 0x40, program_header_size, 1, 0, 0, 0)
    return bytes(buffer)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True, help="output directory")
    args = parser.parse_args()

    args.out.mkdir(parents=True, exist_ok=True)
    (args.out / "synthetic.nso").write_bytes(build_nso())
    (args.out / "synthetic.nro").write_bytes(build_nro())
    (args.out / "synthetic.elf").write_bytes(build_elf64())
    print(f"wrote synthetic fixtures to {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
