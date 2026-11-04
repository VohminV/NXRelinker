#include "synthetic_builders.h"

#include "nxrelinker/binary/binary_format.h"

#include <gtest/gtest.h>

#include <vector>

using nxrelinker::binary::BinaryFormat;
using nxrelinker::binary::detect_format;
using nxrelinker::binary::format_name;

TEST(FormatDetection, DetectsNsoAtOffsetZero) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nso();

    const auto detection = detect_format(nxrelinker::test::bytes_view(bytes));

    EXPECT_EQ(detection.format, BinaryFormat::nso);
    ASSERT_TRUE(detection.magic_offset.has_value());
    EXPECT_EQ(*detection.magic_offset, 0U);
    EXPECT_EQ(detection.magic, "NSO0");
}

TEST(FormatDetection, DetectsNroAtOffset0x10) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_nro();

    const auto detection = detect_format(nxrelinker::test::bytes_view(bytes));

    EXPECT_EQ(detection.format, BinaryFormat::nro);
    ASSERT_TRUE(detection.magic_offset.has_value());
    EXPECT_EQ(*detection.magic_offset, 0x10U);
    EXPECT_EQ(detection.magic, "NRO0");
}

TEST(FormatDetection, DoesNotAcceptNroMagicAtOffsetZero) {
    std::vector<std::byte> misplaced = nxrelinker::test::make_buffer(0x100);
    nxrelinker::test::write_magic(misplaced, 0, "NRO0");

    const auto detection = detect_format(nxrelinker::test::bytes_view(misplaced));

    EXPECT_EQ(detection.format, BinaryFormat::unknown);
    EXPECT_FALSE(detection.magic_offset.has_value());
}

TEST(FormatDetection, DetectsElf) {
    std::vector<std::byte> bytes = nxrelinker::test::make_valid_elf64();

    const auto detection = detect_format(nxrelinker::test::bytes_view(bytes));

    EXPECT_EQ(detection.format, BinaryFormat::elf64);
    ASSERT_TRUE(detection.magic_offset.has_value());
    EXPECT_EQ(*detection.magic_offset, 0U);
    EXPECT_EQ(detection.magic, "ELF");
}

TEST(FormatDetection, RejectsEmptyAndShortInputs) {
    std::vector<std::byte> empty;
    EXPECT_EQ(detect_format(nxrelinker::test::bytes_view(empty)).format,
              BinaryFormat::unknown);

    std::vector<std::byte> short_input = nxrelinker::test::make_buffer(2);
    EXPECT_EQ(detect_format(nxrelinker::test::bytes_view(short_input)).format,
              BinaryFormat::unknown);
}

TEST(FormatDetection, FormatNames) {
    EXPECT_EQ(format_name(BinaryFormat::nso), "NSO0");
    EXPECT_EQ(format_name(BinaryFormat::nro), "NRO0");
    EXPECT_EQ(format_name(BinaryFormat::elf64), "ELF64");
    EXPECT_EQ(format_name(BinaryFormat::unknown), "unknown");
}
