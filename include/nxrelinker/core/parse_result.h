#pragma once

#include "nxrelinker/diagnostics/diagnostic.h"

#include <optional>
#include <utility>

namespace nxrelinker {

template <typename T>
struct ParseResult {
    std::optional<T> value;
    diagnostics::DiagnosticBag diagnostics;

    [[nodiscard]] explicit operator bool() const noexcept { return value.has_value(); }

    [[nodiscard]] T& operator*() { return *value; }
    [[nodiscard]] const T& operator*() const { return *value; }
    [[nodiscard]] T* operator->() { return &*value; }
    [[nodiscard]] const T* operator->() const { return &*value; }

    static ParseResult success(T parsed, diagnostics::DiagnosticBag bag = {}) {
        ParseResult result;
        result.value = std::move(parsed);
        result.diagnostics = std::move(bag);
        return result;
    }

    static ParseResult failure(diagnostics::DiagnosticBag bag) {
        ParseResult result;
        result.diagnostics = std::move(bag);
        return result;
    }
};

} // namespace nxrelinker
