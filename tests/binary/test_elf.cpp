#include "synthetic_builders.h"

#include "nxrelinker/binary/elf.h"

#include <gtest/gtest.h>

#include <vector>

using nxrelinker::binary::parse_elf64;

TEST(ElfHeader, ParsesValidAArch64Header) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    EXPECT_EQ(result.diagnostics.count(nxrelinker::diagnostics::Severity::warning), 0U);
    EXPECT_EQ(result->machine, 183U);
    EXPECT_EQ(result->entry_point, 0x400000U);
    EXPECT_EQ(result->program_header_count, 1U);
    EXPECT_EQ(result->program_header_entry_size, 56U);
    EXPECT_EQ(result->elf_class, nxrelinker::binary::ElfClass::elf64);
    EXPECT_EQ(result->data, nxrelinker::binary::ElfData::little_endian);
    EXPECT_EQ(result->identification_version, 1U);
}

TEST(ElfHeader, WarnsOnUnknownIdentificationVersion) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();
    bytes[6] = static_cast<std::byte>(2);

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "elf.header.unexpected_identification_version") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(ElfHeader, RejectsElf32) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();
    bytes[4] = static_cast<std::byte>(1);

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_FALSE(result.diagnostics.items().empty());
    EXPECT_EQ(result.diagnostics.items()[0].code, "elf.header.unsupported_class");
}

TEST(ElfHeader, RejectsBigEndianInput) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();
    bytes[5] = static_cast<std::byte>(2);

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_FALSE(result.diagnostics.items().empty());
    EXPECT_EQ(result.diagnostics.items()[0].code, "elf.header.unsupported_endianness");
}

TEST(ElfHeader, WarnsOnNonAArch64Machine) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();
    nxrelinker::test::write_u16_le(bytes, 0x12, 62);

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "elf.header.unexpected_machine") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(ElfHeader, RejectsProgramHeaderTableOutsideFile) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();
    nxrelinker::test::write_u16_le(bytes, 0x38, 4);

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_TRUE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "elf.program_headers.out_of_range") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(ElfHeader, RejectsTruncatedHeader) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();
    bytes.resize(0x20);

    auto result = parse_elf64(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_FALSE(result.diagnostics.items().empty());
    EXPECT_EQ(result.diagnostics.items()[0].code, "elf.header.truncated");
}
