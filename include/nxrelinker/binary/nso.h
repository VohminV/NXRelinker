#pragma once

#include "nxrelinker/core/byte_span.h"
#include "nxrelinker/core/parse_result.h"

#include <array>
#include <cstdint>

namespace nxrelinker::binary {

inline constexpr std::uint32_t kNsoHeaderSize = 0x100;

struct NsoSegment {
    std::uint32_t file_offset = 0;
    std::uint32_t memory_offset = 0;
    std::uint32_t memory_size = 0;
    std::uint32_t compressed_file_size = 0;
    bool compressed = false;
};

struct NsoHeader {
    std::uint32_t version = 0;
    std::uint32_t flags = 0;
    NsoSegment text;
    NsoSegment ro;
    NsoSegment data;
    std::uint32_t module_name_offset = 0;
    std::uint32_t module_name_size = 0;
    std::uint32_t bss_size = 0;
    std::array<std::byte, 0x20> module_id{};
    std::uint32_t embedded_offset = 0;
    std::uint32_t embedded_size = 0;
    std::uint32_t dynstr_offset = 0;
    std::uint32_t dynstr_size = 0;
    std::uint32_t dynsym_offset = 0;
    std::uint32_t dynsym_size = 0;
    std::array<std::byte, 0x20> text_hash{};
    std::array<std::byte, 0x20> ro_hash{};
    std::array<std::byte, 0x20> data_hash{};
};

[[nodiscard]] ParseResult<NsoHeader> parse_nso(ByteSpan data);

} // namespace nxrelinker::binary
