#pragma once

#include <cstdint>
#include <limits>

namespace nxrelinker {

[[nodiscard]] constexpr bool range_within(std::uint64_t offset, std::uint64_t length,
                                          std::uint64_t total) noexcept {
    return offset <= total && length <= total - offset;
}

[[nodiscard]] constexpr std::uint64_t saturating_add(std::uint64_t left,
                                                     std::uint64_t right) noexcept {
    if (right > std::numeric_limits<std::uint64_t>::max() - left) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return left + right;
}

[[nodiscard]] constexpr bool ranges_overlap(std::uint64_t first_offset, std::uint64_t first_length,
                                            std::uint64_t second_offset,
                                            std::uint64_t second_length) noexcept {
    if (first_length == 0 || second_length == 0) {
        return false;
    }

    if (first_offset <= second_offset) {
        return second_offset - first_offset < first_length;
    }
    return first_offset - second_offset < second_length;
}

} // namespace nxrelinker
