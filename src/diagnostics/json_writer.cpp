#include "nxrelinker/diagnostics/json_writer.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace nxrelinker::diagnostics {

namespace {

constexpr char kHexDigits[] = "0123456789abcdef";

void append_unicode_escape(std::string& output, std::uint32_t codepoint) {
    constexpr std::uint32_t kFourBits = 0xFU;
    constexpr std::uint32_t kSixteen = 16U;

    output.append("\\u");
    for (int shift = 12; shift >= 0; shift -= 4) {
        const std::uint32_t nibble =
            (codepoint >> static_cast<std::uint32_t>(shift)) & kFourBits;
        output.push_back(kHexDigits[nibble % kSixteen]);
    }
}

} // namespace

std::string json_escape(std::string_view text) {
    std::string output;
    output.reserve(text.size() + 2);

    for (const char character : text) {
        const auto value = static_cast<unsigned char>(character);
        switch (value) {
        case '"':
            output.append("\\\"");
            break;
        case '\\':
            output.append("\\\\");
            break;
        case '\b':
            output.append("\\b");
            break;
        case '\f':
            output.append("\\f");
            break;
        case '\n':
            output.append("\\n");
            break;
        case '\r':
            output.append("\\r");
            break;
        case '\t':
            output.append("\\t");
            break;
        default:
            if (value < 0x20U) {
                append_unicode_escape(output, static_cast<std::uint32_t>(value));
            } else {
                output.push_back(character);
            }
            break;
        }
    }

    return output;
}

void JsonWriter::start_object() {
    before_element();
    buffer_.push_back('{');
    stack_.push_back(Frame{true, true});
    after_key_ = false;
}

void JsonWriter::end_object() {
    if (stack_.empty() || !stack_.back().object) {
        return;
    }
    const bool was_empty = stack_.back().first;
    stack_.pop_back();
    if (!was_empty) {
        buffer_.push_back('\n');
        write_indent();
    }
    buffer_.push_back('}');
    after_key_ = false;
}

void JsonWriter::start_array() {
    before_element();
    buffer_.push_back('[');
    stack_.push_back(Frame{false, true});
    after_key_ = false;
}

void JsonWriter::end_array() {
    if (stack_.empty() || stack_.back().object) {
        return;
    }
    const bool was_empty = stack_.back().first;
    stack_.pop_back();
    if (!was_empty) {
        buffer_.push_back('\n');
        write_indent();
    }
    buffer_.push_back(']');
    after_key_ = false;
}

void JsonWriter::key(std::string_view name) {
    if (stack_.empty() || !stack_.back().object || after_key_) {
        return;
    }
    before_element();
    buffer_.push_back('"');
    buffer_.append(json_escape(name));
    buffer_.append("\": ");
    after_key_ = true;
}

void JsonWriter::string(std::string_view value) {
    before_element();
    buffer_.push_back('"');
    buffer_.append(json_escape(value));
    buffer_.push_back('"');
    after_key_ = false;
}

void JsonWriter::number(std::uint64_t value) {
    before_element();
    buffer_.append(std::to_string(value));
    after_key_ = false;
}

void JsonWriter::boolean(bool value) {
    before_element();
    buffer_.append(value ? "true" : "false");
    after_key_ = false;
}

void JsonWriter::null() {
    before_element();
    buffer_.append("null");
    after_key_ = false;
}

std::string JsonWriter::finish() {
    return std::move(buffer_);
}

void JsonWriter::before_element() {
    if (after_key_) {
        after_key_ = false;
        return;
    }

    if (stack_.empty()) {
        return;
    }

    Frame& frame = stack_.back();
    if (!frame.first) {
        buffer_.push_back(',');
    }
    frame.first = false;

    buffer_.push_back('\n');
    write_indent();
}

void JsonWriter::write_indent() {
    buffer_.append(stack_.size() * 2U, ' ');
}

} // namespace nxrelinker::diagnostics
