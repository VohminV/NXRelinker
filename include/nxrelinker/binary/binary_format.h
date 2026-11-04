#pragma once

#include "nxrelinker/core/byte_span.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace nxrelinker::binary {

enum class BinaryFormat : std::uint8_t {
    unknown,
    nso,
    nro,
    elf64,
};

[[nodiscard]] std::string_view format_name(BinaryFormat format) noexcept;

struct FormatDetection {
    BinaryFormat format = BinaryFormat::unknown;
    std::optional<std::uint32_t> magic_offset;
    std::string magic;
};

[[nodiscard]] FormatDetection detect_format(ByteSpan data) noexcept;

} // namespace nxrelinker::binary
