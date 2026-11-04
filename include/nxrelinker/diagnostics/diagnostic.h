#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace nxrelinker::diagnostics {

enum class Severity : std::uint8_t {
    info,
    warning,
    error,
};

[[nodiscard]] std::string_view to_string(Severity severity) noexcept;
[[nodiscard]] std::string_view json_name(Severity severity) noexcept;

struct Diagnostic {
    Severity severity = Severity::info;
    std::string code;
    std::string message;
    std::optional<std::uint64_t> offset;
};

class DiagnosticBag {
public:
    void add(Severity severity, std::string code, std::string message,
             std::optional<std::uint64_t> offset = std::nullopt);
    void info(std::string code, std::string message,
              std::optional<std::uint64_t> offset = std::nullopt);
    void warning(std::string code, std::string message,
                 std::optional<std::uint64_t> offset = std::nullopt);
    void error(std::string code, std::string message,
               std::optional<std::uint64_t> offset = std::nullopt);

    [[nodiscard]] bool has_errors() const noexcept;
    [[nodiscard]] bool has_warnings() const noexcept;
    [[nodiscard]] std::size_t count(Severity severity) const noexcept;
    [[nodiscard]] const std::vector<Diagnostic>& items() const noexcept;

private:
    std::vector<Diagnostic> items_;
};

} // namespace nxrelinker::diagnostics
