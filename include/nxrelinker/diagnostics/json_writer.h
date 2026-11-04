#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace nxrelinker::diagnostics {

[[nodiscard]] std::string json_escape(std::string_view text);

class JsonWriter {
public:
    void start_object();
    void end_object();
    void start_array();
    void end_array();
    void key(std::string_view name);
    void string(std::string_view value);
    void number(std::uint64_t value);
    void boolean(bool value);
    void null();

    [[nodiscard]] std::string finish();

private:
    struct Frame {
        bool object = false;
        bool first = true;
    };

    void before_element();
    void write_indent();

    std::string buffer_;
    std::vector<Frame> stack_;
    bool after_key_ = false;
};

} // namespace nxrelinker::diagnostics
