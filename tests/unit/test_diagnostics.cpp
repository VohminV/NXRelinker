#include "nxrelinker/core/hex.h"
#include "nxrelinker/diagnostics/diagnostic.h"
#include "nxrelinker/diagnostics/json_writer.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <string>
#include <vector>

using nxrelinker::diagnostics::DiagnosticBag;
using nxrelinker::diagnostics::Severity;
using nxrelinker::diagnostics::json_escape;

TEST(Diagnostics, TracksSeveritiesAndOffsets) {
    DiagnosticBag bag;
    bag.info("test.info", "informational", 1);
    bag.warning("test.warning", "warning", 2);
    bag.error("test.error", "failure", 3);

    EXPECT_TRUE(bag.has_errors());
    EXPECT_TRUE(bag.has_warnings());
    EXPECT_EQ(bag.count(Severity::info), 1U);
    EXPECT_EQ(bag.count(Severity::warning), 1U);
    EXPECT_EQ(bag.count(Severity::error), 1U);
    ASSERT_EQ(bag.items().size(), 3U);
    ASSERT_TRUE(bag.items()[1].offset.has_value());
    EXPECT_EQ(*bag.items()[1].offset, 2U);
}

TEST(Diagnostics, EscapesJsonStrings) {
    const std::string input("\"\\\n\t\x01", 5);
    EXPECT_EQ(json_escape(input), "\\\"\\\\\\n\\t\\u0001");
}

TEST(Diagnostics, WritesJsonWithNestedStructures) {
    nxrelinker::diagnostics::JsonWriter writer;
    writer.start_object();
    writer.key("name");
    writer.string("value");
    writer.key("items");
    writer.start_array();
    writer.number(1);
    writer.boolean(true);
    writer.null();
    writer.end_array();
    writer.end_object();

    const std::string output = writer.finish();
    EXPECT_NE(output.find("\"name\": \"value\""), std::string::npos);
    EXPECT_NE(output.find("\"items\": ["), std::string::npos);
    EXPECT_EQ(output.front(), '{');
    EXPECT_EQ(output.back(), '}');
}

TEST(Hex, EncodesBytes) {
    const std::vector<std::byte> bytes{std::byte{0x00}, std::byte{0xAB}, std::byte{0xFF}};
    EXPECT_EQ(nxrelinker::to_hex(nxrelinker::ByteSpan(bytes)), "00abff");
}
