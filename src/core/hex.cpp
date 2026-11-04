#include "nxrelinker/core/hex.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace nxrelinker {

namespace {

constexpr char kHexDigits[] = "0123456789abcdef";

char hex_digit(std::uint8_t value) {
    return kHexDigits[value & 0x0FU];
}

void append_hex_byte(std::string& output, std::uint8_t value) {
    output.push_back(hex_digit(static_cast<std::uint8_t>(value >> 4U)));
    output.push_back(hex_digit(value));
}

} // namespace

std::string to_hex(ByteSpan bytes) {
    std::string output;
    output.reserve(bytes.size() * 2);
    for (const std::byte value : bytes) {
        append_hex_byte(output, std::to_integer<std::uint8_t>(value));
    }
    return output;
}

std::string to_hex(std::string_view bytes) {
    std::string output;
    output.reserve(bytes.size() * 2);
    for (const char value : bytes) {
        append_hex_byte(output, static_cast<std::uint8_t>(value));
    }
    return output;
}

} // namespace nxrelinker
