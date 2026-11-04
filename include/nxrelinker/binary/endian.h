#pragma once

#include "nxrelinker/core/byte_span.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string_view>

namespace nxrelinker::binary {

[[nodiscard]] inline bool has_bytes(ByteSpan data, std::size_t offset,
                                    std::size_t length) noexcept {
    return offset <= data.size() && length <= data.size() - offset;
}

[[nodiscard]] inline bool matches_magic(ByteSpan data, std::size_t offset,
                                        std::string_view magic) noexcept {
    if (magic.empty()) {
        return true;
    }
    return has_bytes(data, offset, magic.size()) &&
           std::memcmp(data.data() + offset, magic.data(), magic.size()) == 0;
}

[[nodiscard]] inline std::optional<std::uint16_t> read_u16_le(ByteSpan data,
                                                              std::size_t offset) noexcept {
    if (!has_bytes(data, offset, sizeof(std::uint16_t))) {
        return std::nullopt;
    }
    return static_cast<std::uint16_t>(std::to_integer<std::uint16_t>(data[offset])) |
           static_cast<std::uint16_t>(std::to_integer<std::uint16_t>(data[offset + 1]) << 8U);
}

[[nodiscard]] inline std::optional<std::uint32_t> read_u32_le(ByteSpan data,
                                                              std::size_t offset) noexcept {
    if (!has_bytes(data, offset, sizeof(std::uint32_t))) {
        return std::nullopt;
    }
    const std::uint32_t b0 = std::to_integer<std::uint32_t>(data[offset]);
    const std::uint32_t b1 = std::to_integer<std::uint32_t>(data[offset + 1]);
    const std::uint32_t b2 = std::to_integer<std::uint32_t>(data[offset + 2]);
    const std::uint32_t b3 = std::to_integer<std::uint32_t>(data[offset + 3]);
    return b0 | (b1 << 8U) | (b2 << 16U) | (b3 << 24U);
}

[[nodiscard]] inline std::optional<std::uint64_t> read_u64_le(ByteSpan data,
                                                              std::size_t offset) noexcept {
    if (!has_bytes(data, offset, sizeof(std::uint64_t))) {
        return std::nullopt;
    }
    std::uint64_t value = 0;
    for (std::size_t index = 0; index < sizeof(std::uint64_t); ++index) {
        value |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(data[offset + index]))
                 << (index * 8U);
    }
    return value;
}

} // namespace nxrelinker::binary
