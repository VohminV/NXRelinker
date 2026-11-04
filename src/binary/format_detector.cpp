#include "nxrelinker/binary/binary_format.h"

#include "nxrelinker/binary/endian.h"

#include <cstdint>
#include <string>

namespace nxrelinker::binary {

namespace {

constexpr std::string_view kNsoMagic = "NSO0";
constexpr std::string_view kNroMagic = "NRO0";
constexpr std::string_view kElfMagic = "\x7f"
                                       "ELF";
constexpr std::uint32_t kNroSignatureOffset = 0x10;

} // namespace

std::string_view format_name(BinaryFormat format) noexcept {
    switch (format) {
    case BinaryFormat::nso:
        return "NSO0";
    case BinaryFormat::nro:
        return "NRO0";
    case BinaryFormat::elf64:
        return "ELF64";
    case BinaryFormat::unknown:
        break;
    }
    return "unknown";
}

FormatDetection detect_format(ByteSpan data) noexcept {
    if (matches_magic(data, 0, kNsoMagic)) {
        return FormatDetection{BinaryFormat::nso, 0, std::string(kNsoMagic)};
    }
    if (matches_magic(data, kNroSignatureOffset, kNroMagic)) {
        return FormatDetection{BinaryFormat::nro, kNroSignatureOffset, std::string(kNroMagic)};
    }
    if (matches_magic(data, 0, kElfMagic)) {
        return FormatDetection{BinaryFormat::elf64, 0, "ELF"};
    }
    return FormatDetection{};
}

} // namespace nxrelinker::binary
