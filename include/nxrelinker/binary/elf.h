#pragma once

#include "nxrelinker/core/byte_span.h"
#include "nxrelinker/core/parse_result.h"

#include <cstdint>

namespace nxrelinker::binary {

inline constexpr std::uint16_t kElfMachineAArch64 = 183;
inline constexpr std::uint64_t kElf64HeaderSize = 0x40;

enum class ElfClass : std::uint8_t {
    none = 0,
    elf32 = 1,
    elf64 = 2,
};

enum class ElfData : std::uint8_t {
    none = 0,
    little_endian = 1,
    big_endian = 2,
};

enum class ElfType : std::uint16_t {
    none = 0,
    relocatable = 1,
    executable = 2,
    shared_object = 3,
    core = 4,
};

struct Elf64Header {
    ElfClass elf_class = ElfClass::none;
    ElfData data = ElfData::none;
    std::uint8_t identification_version = 0;
    std::uint8_t os_abi = 0;
    std::uint8_t abi_version = 0;
    ElfType type = ElfType::none;
    std::uint16_t machine = 0;
    std::uint32_t version = 0;
    std::uint64_t entry_point = 0;
    std::uint64_t program_header_offset = 0;
    std::uint64_t section_header_offset = 0;
    std::uint32_t flags = 0;
    std::uint16_t header_size = 0;
    std::uint16_t program_header_entry_size = 0;
    std::uint16_t program_header_count = 0;
    std::uint16_t section_header_entry_size = 0;
    std::uint16_t section_header_count = 0;
    std::uint16_t section_name_index = 0;
};

[[nodiscard]] ParseResult<Elf64Header> parse_elf64(ByteSpan data);

} // namespace nxrelinker::binary
