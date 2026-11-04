#include "synthetic_builders.h"

#include "nxrelinker/binary/nso.h"
#include "nxrelinker/diagnostics/diagnostic.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

using nxrelinker::binary::parse_nso;
using nxrelinker::diagnostics::Severity;

TEST(NsoHeader, ParsesValidHeader) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    EXPECT_EQ(result.diagnostics.count(Severity::warning), 0U);

    EXPECT_EQ(result->version, 0U);
    EXPECT_EQ(result->flags, 0U);
    EXPECT_EQ(result->text.file_offset, 0x100U);
    EXPECT_EQ(result->text.memory_offset, 0x0U);
    EXPECT_EQ(result->text.memory_size, 0x40U);
    EXPECT_FALSE(result->text.compressed);
    EXPECT_EQ(result->ro.file_offset, 0x140U);
    EXPECT_EQ(result->data.memory_offset, 0x2000U);
    EXPECT_EQ(result->bss_size, 0x10U);
    EXPECT_EQ(result->module_name_offset, 0x100U);
    EXPECT_EQ(std::to_integer<unsigned>(result->module_id[0]), 0U);
    EXPECT_EQ(std::to_integer<unsigned>(result->module_id[31]), 31U);
}

TEST(NsoHeader, ParsesCompressedSegment) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x0C, 0x01);
    nxrelinker::test::write_u32_le(bytes, 0x60, 0x20);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    EXPECT_TRUE(result->text.compressed);
    EXPECT_EQ(result->text.compressed_file_size, 0x20U);
    EXPECT_FALSE(result->ro.compressed);
    EXPECT_FALSE(result->data.compressed);
}

TEST(NsoHeader, RejectsCompressedSegmentWithZeroSize) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x0C, 0x01);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_TRUE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nso.segment.compressed_size_zero") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NsoHeader, RejectsTruncatedHeader) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    bytes.resize(0x80);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_EQ(result.diagnostics.items().size(), 1U);
    EXPECT_EQ(result.diagnostics.items()[0].code, "nso.header.truncated");
}

TEST(NsoHeader, RejectsInvalidMagic) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_magic(bytes, 0x00, "NSO1");

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_FALSE(result.diagnostics.items().empty());
    EXPECT_EQ(result.diagnostics.items()[0].code, "nso.header.magic");
}

TEST(NsoHeader, DetectsOutOfRangeSegment) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x18, 0x1000);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nso.segment.file_out_of_range") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NsoHeader, DetectsOverlappingFileRanges) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x20, 0x100);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nso.segment.file_overlap") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NsoHeader, WarnsOnUnknownFlags) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x0C, 0x100);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nso.header.unknown_flags") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NsoHeader, WarnsOnAddressSpaceOverflow) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x34, 0xFFFFF000);
    nxrelinker::test::write_u32_le(bytes, 0x38, 0x20);
    nxrelinker::test::write_u32_le(bytes, 0x3C, 0x2000);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nso.bss.address_space_overflow") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NsoHeader, RejectsModuleNameOutsideFile) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    nxrelinker::test::write_u32_le(bytes, 0x1C, 0x100);
    nxrelinker::test::write_u32_le(bytes, 0x2C, 0x1000);

    auto result = parse_nso(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_TRUE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nso.module_name.out_of_range") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}
