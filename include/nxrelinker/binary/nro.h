#pragma once

#include "nxrelinker/core/byte_span.h"
#include "nxrelinker/core/parse_result.h"

#include <array>
#include <cstdint>

namespace nxrelinker::binary {

inline constexpr std::uint32_t kNroHeaderSize = 0x80;
inline constexpr std::uint32_t kNroMagicOffset = 0x10;

struct NroSegment {
    std::uint32_t memory_offset = 0;
    std::uint32_t size = 0;
};

struct NroHeader {
    std::uint32_t rocrt_module_header_location = 0;
    std::uint32_t version = 0;
    std::uint32_t file_size = 0;
    std::uint32_t flags = 0;
    NroSegment text;
    NroSegment ro;
    NroSegment data;
    std::uint32_t bss_size = 0;
    std::uint32_t additional_header_offset = 0;
    std::array<std::byte, 0x20> module_id{};
    std::uint32_t dso_handle_offset = 0;
    std::uint32_t reserved = 0;
    std::uint32_t embedded_offset = 0;
    std::uint32_t embedded_size = 0;
    std::uint32_t dynstr_offset = 0;
    std::uint32_t dynstr_size = 0;
    std::uint32_t dynsym_offset = 0;
    std::uint32_t dynsym_size = 0;
};

[[nodiscard]] ParseResult<NroHeader> parse_nro(ByteSpan data);

} // namespace nxrelinker::binary
