#include "nxrelinker/core/checked_math.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

using nxrelinker::range_within;
using nxrelinker::ranges_overlap;
using nxrelinker::saturating_add;

TEST(CheckedMath, RangeWithinHandlesEdges) {
    EXPECT_TRUE(range_within(0, 0, 0));
    EXPECT_TRUE(range_within(0, 10, 10));
    EXPECT_TRUE(range_within(5, 5, 10));
    EXPECT_FALSE(range_within(6, 5, 10));
    EXPECT_FALSE(range_within(11, 0, 10));
    EXPECT_TRUE(range_within(std::numeric_limits<std::uint64_t>::max(), 0,
                             std::numeric_limits<std::uint64_t>::max()));
}

TEST(CheckedMath, RangesOverlapHandlesEmptyAndAdjacentRanges) {
    EXPECT_FALSE(ranges_overlap(0, 0, 0, 10));
    EXPECT_FALSE(ranges_overlap(10, 5, 0, 10));
    EXPECT_TRUE(ranges_overlap(9, 2, 0, 10));
    EXPECT_TRUE(ranges_overlap(0, 10, 9, 2));
    EXPECT_TRUE(ranges_overlap(0, 10, 0, 10));
}

TEST(CheckedMath, RangesOverlapDoesNotWrapAtTheAddressLimit) {
    constexpr auto max = std::numeric_limits<std::uint64_t>::max();
    EXPECT_TRUE(ranges_overlap(max, 2, max, 1));
    EXPECT_FALSE(ranges_overlap(max, 2, 0, 10));
    EXPECT_FALSE(ranges_overlap(0, 10, max - 1, 4));
}

TEST(CheckedMath, SaturatingAddDoesNotWrap) {
    constexpr auto max = std::numeric_limits<std::uint64_t>::max();
    EXPECT_EQ(saturating_add(1, 2), 3U);
    EXPECT_EQ(saturating_add(max - 1, 1), max);
    EXPECT_EQ(saturating_add(max, 1), max);
}
