#include "synthetic_builders.h"

#include "nxrelinker/binary/nro.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

using nxrelinker::binary::parse_nro;

TEST(NroHeader, ParsesValidHeader) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    EXPECT_EQ(result.diagnostics.count(nxrelinker::diagnostics::Severity::warning), 0U);
    EXPECT_EQ(result->version, 0U);
    EXPECT_EQ(result->file_size, 0x200U);
    EXPECT_EQ(result->text.memory_offset, 0x0U);
    EXPECT_EQ(result->text.size, 0x40U);
    EXPECT_EQ(result->ro.memory_offset, 0x40U);
    EXPECT_EQ(result->data.memory_offset, 0x80U);
    EXPECT_EQ(result->data.size, 0x40U);
    EXPECT_EQ(result->bss_size, 0x10U);
    EXPECT_EQ(result->rocrt_module_header_location, 0x8U);
}

TEST(NroHeader, RejectsMagicAtWrongOffset) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    nxrelinker::test::write_magic(bytes, 0x00, "NRO0");
    bytes[0x10] = std::byte{0};

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_FALSE(result.diagnostics.items().empty());
    EXPECT_EQ(result.diagnostics.items()[0].code, "nro.header.magic");
}

TEST(NroHeader, RejectsTruncatedHeader) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    bytes.resize(0x40);

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    EXPECT_FALSE(result);
    ASSERT_FALSE(result.diagnostics.items().empty());
    EXPECT_EQ(result.diagnostics.items()[0].code, "nro.header.truncated");
}

TEST(NroHeader, RejectsDeclaredSizeBeyondFile) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    nxrelinker::test::write_u32_le(bytes, 0x18, 0x1000);

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_TRUE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nro.header.truncated") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NroHeader, ReportsTrailingAssetBytes) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    nxrelinker::test::write_u32_le(bytes, 0x18, 0x180);

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    EXPECT_FALSE(result.diagnostics.has_errors());
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nro.file.trailing_data") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NroHeader, WarnsOnCompressedImage) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    nxrelinker::test::write_u32_le(bytes, 0x1C, 0x02);

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nro.header.compressed") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NroHeader, DetectsOverlappingSegments) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    nxrelinker::test::write_u32_le(bytes, 0x28, 0x00);

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nro.segment.memory_overlap") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(NroHeader, WarnsOnInvalidAdditionalHeader) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    nxrelinker::test::write_u32_le(bytes, 0x3C, 0x100);

    auto result = parse_nro(nxrelinker::test::bytes_view(bytes));

    ASSERT_TRUE(result);
    bool found = false;
    for (const auto& diagnostic : result.diagnostics.items()) {
        if (diagnostic.code == "nro.additional_header.magic") {
            found = true;
        }
    }
    EXPECT_TRUE(found);
}
