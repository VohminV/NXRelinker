#pragma once

#include "nxrelinker/core/byte_span.h"

#include <string>
#include <string_view>

namespace nxrelinker {

[[nodiscard]] std::string to_hex(ByteSpan bytes);
[[nodiscard]] std::string to_hex(std::string_view bytes);

} // namespace nxrelinker
