#pragma once

#include "nxrelinker/binary/binary_format.h"
#include "nxrelinker/diagnostics/diagnostic.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace nxrelinker::binary {

struct SegmentInfo {
    std::string name;
    std::optional<std::uint32_t> file_offset;
    std::optional<std::uint64_t> file_size;
    std::uint64_t memory_offset = 0;
    std::uint64_t memory_size = 0;
    bool compressed = false;
};

struct BinaryInspection {
    std::string path;
    BinaryFormat format = BinaryFormat::unknown;
    std::uint64_t file_size = 0;
    std::optional<std::uint32_t> magic_offset;
    std::string magic;
    std::optional<std::uint32_t> version;
    std::optional<std::uint32_t> flags;
    std::string module_id_hex;
    std::optional<std::uint64_t> bss_size;
    std::optional<std::uint64_t> entry_point;
    std::optional<std::uint16_t> elf_machine;
    std::vector<SegmentInfo> segments;
    diagnostics::DiagnosticBag diagnostics;
};

[[nodiscard]] BinaryInspection inspect_binary(ByteSpan data, std::string_view path = {});
[[nodiscard]] std::string format_text(const BinaryInspection& inspection);
[[nodiscard]] std::string format_json(const BinaryInspection& inspection);

} // namespace nxrelinker::binary
