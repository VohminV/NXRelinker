#pragma once

#include "nxrelinker/core/byte_span.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace nxrelinker::test {

inline ByteSpan bytes_view(const std::vector<std::byte>& bytes) {
    return ByteSpan(bytes.data(), bytes.size());
}

inline std::vector<std::byte> make_buffer(std::size_t size) {
    return std::vector<std::byte>(size, std::byte{0});
}

inline void write_u16_le(std::vector<std::byte>& bytes, std::size_t offset,
                         std::uint16_t value) {
    bytes[offset] = static_cast<std::byte>(value & 0xFFU);
    bytes[offset + 1] = static_cast<std::byte>((value >> 8U) & 0xFFU);
}

inline void write_u32_le(std::vector<std::byte>& bytes, std::size_t offset,
                         std::uint32_t value) {
    for (std::size_t index = 0; index < 4; ++index) {
        bytes[offset + index] =
            static_cast<std::byte>((value >> (index * 8U)) & 0xFFU);
    }
}

inline void write_u64_le(std::vector<std::byte>& bytes, std::size_t offset,
                         std::uint64_t value) {
    for (std::size_t index = 0; index < 8; ++index) {
        bytes[offset + index] =
            static_cast<std::byte>((value >> (index * 8U)) & 0xFFU);
    }
}

inline void write_magic(std::vector<std::byte>& bytes, std::size_t offset,
                        const char* magic) {
    std::memcpy(bytes.data() + offset, magic, 4);
}

inline void write_module_id(std::vector<std::byte>& bytes, std::size_t offset) {
    for (std::size_t index = 0; index < 0x20; ++index) {
        bytes[offset + index] = static_cast<std::byte>(index);
    }
}

inline std::vector<std::byte> make_valid_nso() {
    std::vector<std::byte> bytes = make_buffer(0x180);
    write_magic(bytes, 0x00, "NSO0");
    write_u32_le(bytes, 0x04, 0);
    write_u32_le(bytes, 0x08, 0);
    write_u32_le(bytes, 0x0C, 0);

    write_u32_le(bytes, 0x10, 0x100);
    write_u32_le(bytes, 0x14, 0x0000);
    write_u32_le(bytes, 0x18, 0x40);
    write_u32_le(bytes, 0x20, 0x140);
    write_u32_le(bytes, 0x24, 0x1000);
    write_u32_le(bytes, 0x28, 0x20);
    write_u32_le(bytes, 0x30, 0x160);
    write_u32_le(bytes, 0x34, 0x2000);
    write_u32_le(bytes, 0x38, 0x20);

    write_u32_le(bytes, 0x1C, 0x100);
    write_u32_le(bytes, 0x2C, 0);
    write_u32_le(bytes, 0x3C, 0x10);
    write_module_id(bytes, 0x40);

    return bytes;
}

inline std::vector<std::byte> make_valid_nro() {
    std::vector<std::byte> bytes = make_buffer(0x200);
    write_u32_le(bytes, 0x00, 0x8);
    write_magic(bytes, 0x10, "NRO0");
    write_u32_le(bytes, 0x14, 0);
    write_u32_le(bytes, 0x18, static_cast<std::uint32_t>(bytes.size()));
    write_u32_le(bytes, 0x1C, 0);

    write_u32_le(bytes, 0x20, 0x0000);
    write_u32_le(bytes, 0x24, 0x40);
    write_u32_le(bytes, 0x28, 0x40);
    write_u32_le(bytes, 0x2C, 0x40);
    write_u32_le(bytes, 0x30, 0x80);
    write_u32_le(bytes, 0x34, 0x40);
    write_u32_le(bytes, 0x38, 0x10);
    write_module_id(bytes, 0x40);

    return bytes;
}

inline std::vector<std::byte> make_valid_elf64() {
    constexpr std::size_t kProgramHeaderOffset = 0x40;
    constexpr std::size_t kProgramHeaderSize = 56;
    std::vector<std::byte> bytes = make_buffer(kProgramHeaderOffset + kProgramHeaderSize);

    bytes[0] = static_cast<std::byte>(0x7F);
    bytes[1] = static_cast<std::byte>('E');
    bytes[2] = static_cast<std::byte>('L');
    bytes[3] = static_cast<std::byte>('F');
    bytes[4] = static_cast<std::byte>(2);
    bytes[5] = static_cast<std::byte>(1);
    bytes[6] = static_cast<std::byte>(1);

    write_u16_le(bytes, 0x10, 2);
    write_u16_le(bytes, 0x12, 183);
    write_u32_le(bytes, 0x14, 1);
    write_u64_le(bytes, 0x18, 0x400000);
    write_u64_le(bytes, 0x20, kProgramHeaderOffset);
    write_u64_le(bytes, 0x28, 0);
    write_u32_le(bytes, 0x30, 0);
    write_u16_le(bytes, 0x34, 0x40);
    write_u16_le(bytes, 0x36, static_cast<std::uint16_t>(kProgramHeaderSize));
    write_u16_le(bytes, 0x38, 1);
    write_u16_le(bytes, 0x3A, 0);
    write_u16_le(bytes, 0x3C, 0);
    write_u16_le(bytes, 0x3E, 0);

    return bytes;
}

} // namespace nxrelinker::test
