#include "nxrelinker/binary/nso.h"

#include "nxrelinker/binary/endian.h"
#include "nxrelinker/core/checked_math.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace nxrelinker::binary {

namespace {

constexpr std::uint32_t kFlagTextCompressed = 1U << 0U;
constexpr std::uint32_t kFlagRoCompressed = 1U << 1U;
constexpr std::uint32_t kFlagDataCompressed = 1U << 2U;
constexpr std::uint32_t kFlagExecuteOnlyMemory = 1U << 6U;
constexpr std::uint32_t kFlagZbicCompression = 1U << 7U;
constexpr std::uint32_t kKnownFlags = (1U << 8U) - 1U;

constexpr std::uint64_t kAddressSpaceLimit = 0x1'0000'0000ULL;

struct SegmentDescriptor {
    std::string_view name;
    std::size_t file_offset_field;
    std::size_t memory_offset_field;
    std::size_t memory_size_field;
    std::size_t compressed_size_field;
    std::uint32_t compression_flag;
};

constexpr std::array<SegmentDescriptor, 3> kSegmentDescriptors{{
    {"text", 0x10, 0x14, 0x18, 0x60, kFlagTextCompressed},
    {"ro", 0x20, 0x24, 0x28, 0x64, kFlagRoCompressed},
    {"data", 0x30, 0x34, 0x38, 0x68, kFlagDataCompressed},
}};

void copy_field(ByteSpan data, std::size_t offset, std::array<std::byte, 0x20>& destination) {
    std::copy_n(data.begin() + static_cast<std::ptrdiff_t>(offset), destination.size(),
                destination.begin());
}

void validate_module_name(ByteSpan data, const NsoHeader& header,
                          diagnostics::DiagnosticBag& diagnostics) {
    if (header.module_name_size == 0) {
        return;
    }
    if (!range_within(header.module_name_offset, header.module_name_size, data.size())) {
        diagnostics.error("nso.module_name.out_of_range",
                          "module name lies outside the input file", 0x1C);
        return;
    }
    if (header.module_name_offset != kNsoHeaderSize) {
        diagnostics.warning("nso.module_name.unexpected_offset",
                            "module name offset differs from the header size (0x100)", 0x1C);
    }
}

void validate_dynamic_tables(const NsoHeader& header,
                             diagnostics::DiagnosticBag& diagnostics) {
    const auto check_range = [&](std::uint32_t offset, std::uint32_t size,
                                 std::string_view name, std::size_t field_offset) {
        if (size == 0) {
            return;
        }
        if (!range_within(offset, size, header.ro.memory_size)) {
            diagnostics.warning(std::string("nso.") + std::string(name) + ".out_of_range",
                                std::string(name) + " lies outside the rodata segment",
                                field_offset);
        }
    };

    check_range(header.embedded_offset, header.embedded_size, "embedded", 0x88);
    check_range(header.dynstr_offset, header.dynstr_size, "dynstr", 0x90);
    check_range(header.dynsym_offset, header.dynsym_size, "dynsym", 0x98);
}

void validate_segments(ByteSpan data, const NsoHeader& header,
                       diagnostics::DiagnosticBag& diagnostics) {
    struct Bounds {
        std::uint64_t offset;
        std::uint64_t size;
        std::string_view name;
    };

    std::array<Bounds, 3> file_ranges{};
    std::array<Bounds, 3> memory_ranges{};
    std::size_t file_range_count = 0;
    std::size_t memory_range_count = 0;

    const std::array<const NsoSegment*, 3> segments{&header.text, &header.ro, &header.data};

    for (std::size_t index = 0; index < segments.size(); ++index) {
        const NsoSegment& segment = *segments[index];
        const SegmentDescriptor& descriptor = kSegmentDescriptors[index];

        std::uint64_t physical_file_size = segment.memory_size;
        if (segment.compressed) {
            physical_file_size = segment.compressed_file_size;
            if (physical_file_size == 0 && segment.memory_size != 0) {
                diagnostics.error("nso.segment.compressed_size_zero",
                                  std::string(descriptor.name) +
                                      " is marked compressed but has zero compressed size",
                                  descriptor.compressed_size_field);
            }
        } else if (segment.compressed_file_size != 0 &&
                   segment.compressed_file_size != segment.memory_size) {
            diagnostics.warning(
                "nso.segment.compressed_size_ignored",
                std::string(descriptor.name) +
                    " is not compressed; the compressed-size field is ignored during loading",
                descriptor.compressed_size_field);
        }

        if (physical_file_size != 0) {
            if (!range_within(segment.file_offset, physical_file_size, data.size())) {
                diagnostics.error("nso.segment.file_out_of_range",
                                  std::string(descriptor.name) +
                                      " file range exceeds the input size",
                                  descriptor.file_offset_field);
            } else {
                file_ranges[file_range_count++] =
                    Bounds{segment.file_offset, physical_file_size, descriptor.name};
            }
        }

        if (segment.memory_size != 0) {
            memory_ranges[memory_range_count++] = Bounds{segment.memory_offset, segment.memory_size,
                                                         descriptor.name};
        }
    }

    for (std::size_t first = 0; first < file_range_count; ++first) {
        for (std::size_t second = first + 1; second < file_range_count; ++second) {
            if (ranges_overlap(file_ranges[first].offset, file_ranges[first].size,
                               file_ranges[second].offset, file_ranges[second].size)) {
                diagnostics.error("nso.segment.file_overlap",
                                  std::string(file_ranges[first].name) + " and " +
                                      std::string(file_ranges[second].name) +
                                      " overlap in the file",
                                  0x10);
            }
        }
    }

    for (std::size_t first = 0; first < memory_range_count; ++first) {
        for (std::size_t second = first + 1; second < memory_range_count; ++second) {
            if (ranges_overlap(memory_ranges[first].offset, memory_ranges[first].size,
                               memory_ranges[second].offset, memory_ranges[second].size)) {
                diagnostics.error("nso.segment.memory_overlap",
                                  std::string(memory_ranges[first].name) + " and " +
                                      std::string(memory_ranges[second].name) +
                                      " overlap in memory",
                                  0x14);
            }
        }
    }

    const std::uint64_t data_end = saturating_add(
        saturating_add(header.data.memory_offset, header.data.memory_size), header.bss_size);
    if (data_end > kAddressSpaceLimit) {
        diagnostics.warning("nso.bss.address_space_overflow",
                            "data plus bss extends beyond the 32-bit module address space", 0x3C);
    }
}

} // namespace

ParseResult<NsoHeader> parse_nso(ByteSpan data) {
    diagnostics::DiagnosticBag diagnostics;

    if (!has_bytes(data, 0, 4)) {
        diagnostics.error("nso.header.truncated",
                          "input is too small to contain an NSO signature", 0);
        return ParseResult<NsoHeader>::failure(std::move(diagnostics));
    }

    if (!matches_magic(data, 0, "NSO0")) {
        diagnostics.error("nso.header.magic", "missing NSO0 signature", 0);
        return ParseResult<NsoHeader>::failure(std::move(diagnostics));
    }

    if (!has_bytes(data, 0, kNsoHeaderSize)) {
        diagnostics.error("nso.header.truncated",
                          "input is smaller than the 0x100-byte NSO header", 0);
        return ParseResult<NsoHeader>::failure(std::move(diagnostics));
    }

    NsoHeader header;
    header.version = *read_u32_le(data, 0x04);
    header.flags = *read_u32_le(data, 0x0C);

    const std::array<NsoSegment*, 3> segments{&header.text, &header.ro, &header.data};
    for (std::size_t index = 0; index < segments.size(); ++index) {
        const SegmentDescriptor& descriptor = kSegmentDescriptors[index];
        NsoSegment& segment = *segments[index];
        segment.file_offset = *read_u32_le(data, descriptor.file_offset_field);
        segment.memory_offset = *read_u32_le(data, descriptor.memory_offset_field);
        segment.memory_size = *read_u32_le(data, descriptor.memory_size_field);
        segment.compressed_file_size = *read_u32_le(data, descriptor.compressed_size_field);
        segment.compressed = (header.flags & descriptor.compression_flag) != 0;
    }

    header.module_name_offset = *read_u32_le(data, 0x1C);
    header.module_name_size = *read_u32_le(data, 0x2C);
    header.bss_size = *read_u32_le(data, 0x3C);
    copy_field(data, 0x40, header.module_id);
    header.embedded_offset = *read_u32_le(data, 0x88);
    header.embedded_size = *read_u32_le(data, 0x8C);
    header.dynstr_offset = *read_u32_le(data, 0x90);
    header.dynstr_size = *read_u32_le(data, 0x94);
    header.dynsym_offset = *read_u32_le(data, 0x98);
    header.dynsym_size = *read_u32_le(data, 0x9C);
    copy_field(data, 0xA0, header.text_hash);
    copy_field(data, 0xC0, header.ro_hash);
    copy_field(data, 0xE0, header.data_hash);

    if (header.version != 0) {
        diagnostics.warning("nso.header.unexpected_version",
                            "NSO version is not zero; loader compatibility is unverified", 0x04);
    }

    const std::uint32_t reserved = *read_u32_le(data, 0x08);
    if (reserved != 0) {
        diagnostics.warning("nso.header.reserved_nonzero",
                            "reserved header field is not zero", 0x08);
    }

    if ((header.flags & ~kKnownFlags) != 0) {
        diagnostics.warning("nso.header.unknown_flags",
                            "header contains flags not documented by the current format reference",
                            0x0C);
    }

    if ((header.flags & kFlagZbicCompression) != 0) {
        diagnostics.info("nso.header.zbic_compression",
                         "segments marked compressed use zstd-based ZBIC compression", 0x0C);
    }

    if ((header.flags & kFlagExecuteOnlyMemory) != 0) {
        diagnostics.info("nso.header.execute_only_memory",
                         "the executable requests execute-only text memory", 0x0C);
    }

    validate_module_name(data, header, diagnostics);
    validate_segments(data, header, diagnostics);
    validate_dynamic_tables(header, diagnostics);

    return ParseResult<NsoHeader>::success(std::move(header), std::move(diagnostics));
}

} // namespace nxrelinker::binary
