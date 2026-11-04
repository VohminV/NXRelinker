#include "nxrelinker/binary/nro.h"

#include "nxrelinker/binary/endian.h"
#include "nxrelinker/core/checked_math.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace nxrelinker::binary {

namespace {

constexpr std::uint32_t kFlagAlignedHeader = 1U << 0U;
constexpr std::uint32_t kFlagCompressed = 1U << 1U;
constexpr std::uint32_t kKnownFlags = (1U << 2U) - 1U;
constexpr std::uint64_t kAddressSpaceLimit = 0x1'0000'0000ULL;

void copy_field(ByteSpan data, std::size_t offset, std::array<std::byte, 0x20>& destination) {
    std::copy_n(data.begin() + static_cast<std::ptrdiff_t>(offset), destination.size(),
                destination.begin());
}

void validate_segments(const NroHeader& header, diagnostics::DiagnosticBag& diagnostics) {
    struct Bounds {
        std::uint64_t offset;
        std::uint64_t size;
        std::string_view name;
    };

    const std::array<const NroSegment*, 3> segments{&header.text, &header.ro, &header.data};
    const std::array<std::string_view, 3> names{"text", "ro", "data"};
    const std::array<std::size_t, 3> size_fields{0x24, 0x2C, 0x34};

    std::array<Bounds, 3> ranges{};
    std::size_t range_count = 0;

    for (std::size_t index = 0; index < segments.size(); ++index) {
        const NroSegment& segment = *segments[index];
        if (segment.size == 0) {
            continue;
        }
        if (!range_within(segment.memory_offset, segment.size, kAddressSpaceLimit)) {
            diagnostics.error("nro.segment.memory_out_of_range",
                              std::string(names[index]) +
                                  " memory range exceeds the 32-bit module address space",
                              size_fields[index]);
            continue;
        }
        ranges[range_count++] = Bounds{segment.memory_offset, segment.size, names[index]};
    }

    for (std::size_t first = 0; first < range_count; ++first) {
        for (std::size_t second = first + 1; second < range_count; ++second) {
            if (ranges_overlap(ranges[first].offset, ranges[first].size, ranges[second].offset,
                               ranges[second].size)) {
                diagnostics.error("nro.segment.memory_overlap",
                                  std::string(ranges[first].name) + " and " +
                                      std::string(ranges[second].name) +
                                      " overlap in memory",
                                  0x20);
            }
        }
    }

    const std::uint64_t data_end = saturating_add(
        saturating_add(header.data.memory_offset, header.data.size), header.bss_size);
    if (data_end > kAddressSpaceLimit) {
        diagnostics.warning("nro.bss.address_space_overflow",
                            "data plus bss extends beyond the 32-bit module address space", 0x38);
    }
}

} // namespace

ParseResult<NroHeader> parse_nro(ByteSpan data) {
    diagnostics::DiagnosticBag diagnostics;

    if (!has_bytes(data, kNroMagicOffset, 4)) {
        diagnostics.error("nro.header.truncated",
                          "input is too small to contain an NRO signature at offset 0x10", 0);
        return ParseResult<NroHeader>::failure(std::move(diagnostics));
    }

    if (!matches_magic(data, kNroMagicOffset, "NRO0")) {
        diagnostics.error("nro.header.magic", "missing NRO0 signature at offset 0x10", 0);
        return ParseResult<NroHeader>::failure(std::move(diagnostics));
    }

    if (!has_bytes(data, 0, kNroHeaderSize)) {
        diagnostics.error("nro.header.truncated",
                          "input is smaller than the 0x80-byte NRO header", 0);
        return ParseResult<NroHeader>::failure(std::move(diagnostics));
    }

    NroHeader header;
    header.rocrt_module_header_location = *read_u32_le(data, 0x00);
    header.version = *read_u32_le(data, 0x14);
    header.file_size = *read_u32_le(data, 0x18);
    header.flags = *read_u32_le(data, 0x1C);
    header.text = NroSegment{*read_u32_le(data, 0x20), *read_u32_le(data, 0x24)};
    header.ro = NroSegment{*read_u32_le(data, 0x28), *read_u32_le(data, 0x2C)};
    header.data = NroSegment{*read_u32_le(data, 0x30), *read_u32_le(data, 0x34)};
    header.bss_size = *read_u32_le(data, 0x38);
    header.additional_header_offset = *read_u32_le(data, 0x3C);
    copy_field(data, 0x40, header.module_id);
    header.dso_handle_offset = *read_u32_le(data, 0x60);
    header.reserved = *read_u32_le(data, 0x64);
    header.embedded_offset = *read_u32_le(data, 0x68);
    header.embedded_size = *read_u32_le(data, 0x6C);
    header.dynstr_offset = *read_u32_le(data, 0x70);
    header.dynstr_size = *read_u32_le(data, 0x74);
    header.dynsym_offset = *read_u32_le(data, 0x78);
    header.dynsym_size = *read_u32_le(data, 0x7C);

    if (header.version != 0) {
        diagnostics.warning("nro.header.unexpected_version",
                            "NRO version is not zero; loader compatibility is unverified", 0x14);
    }

    if (header.file_size < kNroHeaderSize) {
        diagnostics.error("nro.header.invalid_size",
                          "NRO total size is smaller than its header", 0x18);
    } else if (header.file_size > data.size()) {
        diagnostics.error("nro.header.truncated",
                          "NRO declares more bytes than the input file contains", 0x18);
    } else if (header.file_size < data.size()) {
        diagnostics.info("nro.file.trailing_data",
                         "input contains bytes after the declared NRO image; this can be the "
                         "optional homebrew asset section",
                         0x18);
    }

    if ((header.flags & ~kKnownFlags) != 0) {
        diagnostics.warning("nro.header.unknown_flags",
                            "header contains flags not documented by the current format reference",
                            0x1C);
    }

    if ((header.flags & kFlagCompressed) != 0) {
        diagnostics.warning("nro.header.compressed",
                            "compressed NRO images are not yet supported for segment extraction",
                            0x1C);
    }

    if ((header.flags & kFlagAlignedHeader) != 0) {
        diagnostics.info("nro.header.aligned", "NRO header is marked as aligned", 0x1C);
    }

    if (header.reserved != 0) {
        diagnostics.warning("nro.header.reserved_nonzero",
                            "reserved NRO header field is not zero", 0x64);
    }

    if (header.additional_header_offset != 0) {
        if (!range_within(header.additional_header_offset, 0x80, data.size())) {
            diagnostics.warning("nro.additional_header.out_of_range",
                                "additional NRO header offset lies outside the input file", 0x3C);
        } else if (!matches_magic(data, header.additional_header_offset, "NRA0")) {
            diagnostics.warning("nro.additional_header.magic",
                                "additional NRO header does not start with NRA0", 0x3C);
        }
    }

    if (header.embedded_size != 0 &&
        !range_within(header.embedded_offset, header.embedded_size, header.ro.size)) {
        diagnostics.warning("nro.embedded.out_of_range",
                            "embedded data lies outside the rodata segment", 0x68);
    }

    validate_segments(header, diagnostics);

    return ParseResult<NroHeader>::success(std::move(header), std::move(diagnostics));
}

} // namespace nxrelinker::binary
