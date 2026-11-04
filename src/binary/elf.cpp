#include "nxrelinker/binary/elf.h"

#include "nxrelinker/binary/endian.h"
#include "nxrelinker/core/checked_math.h"

#include <cstdint>
#include <string>

namespace nxrelinker::binary {

namespace {

constexpr std::uint16_t kElf64ProgramHeaderEntrySize = 56;
constexpr std::uint16_t kElf64SectionHeaderEntrySize = 64;
constexpr std::uint16_t kElfSectionIndexUndefined = 0;
constexpr std::uint16_t kElfSectionIndexExtended = 0xFFFF;

} // namespace

ParseResult<Elf64Header> parse_elf64(ByteSpan data) {
    diagnostics::DiagnosticBag diagnostics;

    if (!has_bytes(data, 0, 4)) {
        diagnostics.error("elf.header.truncated", "input is too small to contain an ELF signature",
                          0);
        return ParseResult<Elf64Header>::failure(std::move(diagnostics));
    }

    if (!matches_magic(data, 0, "\x7f"
                                "ELF")) {
        diagnostics.error("elf.header.magic", "missing ELF signature", 0);
        return ParseResult<Elf64Header>::failure(std::move(diagnostics));
    }

    if (!has_bytes(data, 0, kElf64HeaderSize)) {
        diagnostics.error("elf.header.truncated",
                          "input is smaller than the 0x40-byte ELF64 header", 0);
        return ParseResult<Elf64Header>::failure(std::move(diagnostics));
    }

    Elf64Header header;
    header.elf_class = static_cast<ElfClass>(std::to_integer<std::uint8_t>(data[4]));
    header.data = static_cast<ElfData>(std::to_integer<std::uint8_t>(data[5]));
    header.identification_version = std::to_integer<std::uint8_t>(data[6]);
    header.os_abi = std::to_integer<std::uint8_t>(data[7]);
    header.abi_version = std::to_integer<std::uint8_t>(data[8]);
    header.type = static_cast<ElfType>(*read_u16_le(data, 0x10));
    header.machine = *read_u16_le(data, 0x12);
    header.version = *read_u32_le(data, 0x14);
    header.entry_point = *read_u64_le(data, 0x18);
    header.program_header_offset = *read_u64_le(data, 0x20);
    header.section_header_offset = *read_u64_le(data, 0x28);
    header.flags = *read_u32_le(data, 0x30);
    header.header_size = *read_u16_le(data, 0x34);
    header.program_header_entry_size = *read_u16_le(data, 0x36);
    header.program_header_count = *read_u16_le(data, 0x38);
    header.section_header_entry_size = *read_u16_le(data, 0x3A);
    header.section_header_count = *read_u16_le(data, 0x3C);
    header.section_name_index = *read_u16_le(data, 0x3E);

    if (header.elf_class != ElfClass::elf64) {
        diagnostics.error("elf.header.unsupported_class",
                          "only ELF64 inputs are supported by this build", 4);
        return ParseResult<Elf64Header>::failure(std::move(diagnostics));
    }

    if (header.data != ElfData::little_endian) {
        diagnostics.error("elf.header.unsupported_endianness",
                          "only little-endian ELF inputs are supported", 5);
        return ParseResult<Elf64Header>::failure(std::move(diagnostics));
    }

    if (header.header_size != kElf64HeaderSize) {
        diagnostics.error("elf.header.invalid_size",
                          "ELF64 header size field is not 0x40", 0x34);
    }

    if (header.identification_version != 1) {
        diagnostics.warning("elf.header.unexpected_identification_version",
                            "ELF identification version is not 1", 0x06);
    }

    if (header.version != 1) {
        diagnostics.warning("elf.header.unexpected_version",
                            "ELF version is not 1", 0x14);
    }

    if (header.machine != kElfMachineAArch64) {
        diagnostics.warning("elf.header.unexpected_machine",
                            "ELF machine is not AArch64 (EM_AARCH64 = 183)", 0x12);
    }

    if (header.type != ElfType::executable && header.type != ElfType::shared_object) {
        diagnostics.warning("elf.header.unexpected_type",
                            "ELF type is neither ET_EXEC nor ET_DYN", 0x10);
    }

    if (header.program_header_count != 0) {
        if (header.program_header_entry_size != kElf64ProgramHeaderEntrySize) {
            diagnostics.error("elf.program_headers.invalid_entry_size",
                              "ELF64 program header entry size is not 56", 0x36);
        }
        const std::uint64_t table_size =
            static_cast<std::uint64_t>(header.program_header_count) *
            static_cast<std::uint64_t>(header.program_header_entry_size);
        if (!range_within(header.program_header_offset, table_size, data.size())) {
            diagnostics.error("elf.program_headers.out_of_range",
                              "program header table lies outside the input file", 0x20);
        }
    } else {
        diagnostics.warning("elf.program_headers.empty",
                            "ELF file has no program headers", 0x38);
    }

    if (header.section_header_count != 0) {
        if (header.section_header_entry_size != kElf64SectionHeaderEntrySize) {
            diagnostics.error("elf.section_headers.invalid_entry_size",
                              "ELF64 section header entry size is not 64", 0x3A);
        }
        const std::uint64_t table_size =
            static_cast<std::uint64_t>(header.section_header_count) *
            static_cast<std::uint64_t>(header.section_header_entry_size);
        if (!range_within(header.section_header_offset, table_size, data.size())) {
            diagnostics.error("elf.section_headers.out_of_range",
                              "section header table lies outside the input file", 0x28);
        }
        if (header.section_name_index != kElfSectionIndexUndefined &&
            header.section_name_index != kElfSectionIndexExtended &&
            header.section_name_index >= header.section_header_count) {
            diagnostics.warning("elf.section_headers.invalid_name_index",
                                "section name string table index is out of range", 0x3E);
        }
    }

    return ParseResult<Elf64Header>::success(std::move(header), std::move(diagnostics));
}

} // namespace nxrelinker::binary
