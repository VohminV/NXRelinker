#include "synthetic_builders.h"

#include "nxrelinker/binary/inspection.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(Inspection, ProducesHumanReadableReport) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    const auto inspection = nxrelinker::binary::inspect_binary(
        nxrelinker::test::bytes_view(bytes), "sample.nso");

    EXPECT_EQ(inspection.format, nxrelinker::binary::BinaryFormat::nso);
    EXPECT_FALSE(inspection.diagnostics.has_errors());
    ASSERT_EQ(inspection.segments.size(), 3U);
    EXPECT_EQ(inspection.segments[0].name, "text");
    EXPECT_EQ(inspection.segments[0].file_offset, 0x100U);

    const std::string text = nxrelinker::binary::format_text(inspection);
    EXPECT_NE(text.find("File: sample.nso"), std::string::npos);
    EXPECT_NE(text.find("Format: NSO0"), std::string::npos);
    EXPECT_NE(text.find("Segments:"), std::string::npos);
    EXPECT_NE(text.find("Diagnostics: 0 error(s)"), std::string::npos);
}

TEST(Inspection, ProducesValidLookingJsonAndEscapesPath) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();
    const auto inspection = nxrelinker::binary::inspect_binary(
        nxrelinker::test::bytes_view(bytes), "dir/a\"b\\c.nso");

    const std::string json = nxrelinker::binary::format_json(inspection);
    EXPECT_NE(json.find("\"schema_version\": 1"), std::string::npos);
    EXPECT_NE(json.find("\"format\": \"NSO0\""), std::string::npos);
    EXPECT_NE(json.find("dir/a\\\"b\\\\c.nso"), std::string::npos);
    EXPECT_NE(json.find("\"module_id\": \"000102030405"), std::string::npos);
    EXPECT_NE(json.find("\"segments\": ["), std::string::npos);
    EXPECT_NE(json.find("\"diagnostics\": ["), std::string::npos);
    EXPECT_NE(json.find("\"summary\": {"), std::string::npos);
}

TEST(Inspection, NroSegmentsHaveNoEncodedFileOffsets) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();
    const auto inspection = nxrelinker::binary::inspect_binary(
        nxrelinker::test::bytes_view(bytes), "sample.nro");

    ASSERT_EQ(inspection.segments.size(), 3U);
    EXPECT_FALSE(inspection.segments[0].file_offset.has_value());
    EXPECT_FALSE(inspection.segments[0].file_size.has_value());
    EXPECT_EQ(inspection.segments[0].memory_size, 0x40U);

    const std::string json = nxrelinker::binary::format_json(inspection);
    EXPECT_NE(json.find("\"file_offset\": null"), std::string::npos);
}

TEST(Inspection, UnknownFormatIsAnError) {
    const std::vector<std::byte> bytes{std::byte{0x01}, std::byte{0x02},
                                       std::byte{0x03}, std::byte{0x04}};
    const auto inspection =
        nxrelinker::binary::inspect_binary(nxrelinker::test::bytes_view(bytes), "unknown.bin");

    EXPECT_EQ(inspection.format, nxrelinker::binary::BinaryFormat::unknown);
    EXPECT_TRUE(inspection.diagnostics.has_errors());

    const std::string json = nxrelinker::binary::format_json(inspection);
    EXPECT_NE(json.find("\"format\": \"unknown\""), std::string::npos);
    EXPECT_NE(json.find("format.unknown"), std::string::npos);
}
