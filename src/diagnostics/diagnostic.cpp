#include "nxrelinker/diagnostics/diagnostic.h"

#include <utility>

namespace nxrelinker::diagnostics {

std::string_view to_string(Severity severity) noexcept {
    switch (severity) {
    case Severity::info:
        return "info";
    case Severity::warning:
        return "warning";
    case Severity::error:
        return "error";
    }
    return "unknown";
}

std::string_view json_name(Severity severity) noexcept {
    return to_string(severity);
}

void DiagnosticBag::add(Severity severity, std::string code, std::string message,
                        std::optional<std::uint64_t> offset) {
    items_.push_back(
        Diagnostic{severity, std::move(code), std::move(message), std::move(offset)});
}

void DiagnosticBag::info(std::string code, std::string message,
                         std::optional<std::uint64_t> offset) {
    add(Severity::info, std::move(code), std::move(message), offset);
}

void DiagnosticBag::warning(std::string code, std::string message,
                            std::optional<std::uint64_t> offset) {
    add(Severity::warning, std::move(code), std::move(message), offset);
}

void DiagnosticBag::error(std::string code, std::string message,
                          std::optional<std::uint64_t> offset) {
    add(Severity::error, std::move(code), std::move(message), offset);
}

bool DiagnosticBag::has_errors() const noexcept {
    return count(Severity::error) != 0;
}

bool DiagnosticBag::has_warnings() const noexcept {
    return count(Severity::warning) != 0;
}

std::size_t DiagnosticBag::count(Severity severity) const noexcept {
    std::size_t result = 0;
    for (const Diagnostic& item : items_) {
        if (item.severity == severity) {
            ++result;
        }
    }
    return result;
}

const std::vector<Diagnostic>& DiagnosticBag::items() const noexcept {
    return items_;
}

} // namespace nxrelinker::diagnostics
