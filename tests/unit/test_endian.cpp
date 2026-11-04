#include "nxrelinker/binary/endian.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

using nxrelinker::binary::has_bytes;
using nxrelinker::binary::matches_magic;
using nxrelinker::binary::read_u16_le;
using nxrelinker::binary::read_u32_le;
using nxrelinker::binary::read_u64_le;

TEST(Endian, RejectsOutOfRangeReads) {
    const std::vector<std::byte> bytes(3, std::byte{0xFF});
    EXPECT_FALSE(read_u16_le(nxrelinker::ByteSpan(bytes), 2));
    EXPECT_FALSE(read_u32_le(nxrelinker::ByteSpan(bytes), 0));
    EXPECT_FALSE(read_u64_le(nxrelinker::ByteSpan(bytes), 0));
    EXPECT_FALSE(has_bytes(nxrelinker::ByteSpan(bytes), 4, 0));
}

TEST(Endian, ReadsLittleEndianValues) {
    std::vector<std::byte> bytes(8, std::byte{0});
    bytes[0] = std::byte{0x78};
    bytes[1] = std::byte{0x56};
    bytes[2] = std::byte{0x34};
    bytes[3] = std::byte{0x12};
    bytes[4] = std::byte{0xEF};
    bytes[5] = std::byte{0xCD};
    bytes[6] = std::byte{0xAB};
    bytes[7] = std::byte{0x90};

    EXPECT_EQ(*read_u16_le(nxrelinker::ByteSpan(bytes), 0), 0x5678U);
    EXPECT_EQ(*read_u32_le(nxrelinker::ByteSpan(bytes), 0), 0x12345678U);
    EXPECT_EQ(*read_u64_le(nxrelinker::ByteSpan(bytes), 0), 0x90ABCDEF12345678ULL);
}

TEST(Endian, MatchesMagicSafely) {
    const std::vector<std::byte> bytes{std::byte{'N'}, std::byte{'S'}, std::byte{'O'}};
    EXPECT_TRUE(matches_magic(nxrelinker::ByteSpan(bytes), 0, "NSO"));
    EXPECT_TRUE(matches_magic(nxrelinker::ByteSpan(bytes), 0, ""));
    EXPECT_FALSE(matches_magic(nxrelinker::ByteSpan(bytes), 1, "NSO"));
    EXPECT_FALSE(matches_magic(nxrelinker::ByteSpan(), 0, "N"));
}
