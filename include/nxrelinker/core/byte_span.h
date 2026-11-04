#pragma once

#include <cstddef>
#include <span>

namespace nxrelinker {

using ByteSpan = std::span<const std::byte>;
using MutableByteSpan = std::span<std::byte>;

} // namespace nxrelinker
