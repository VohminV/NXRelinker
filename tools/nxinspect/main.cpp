#include "nxrelinker/binary/inspection.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

enum class OutputFormat {
    text,
    json,
};

struct Options {
    std::string path;
    OutputFormat output_format = OutputFormat::text;
    bool strict = false;
};

void print_usage(std::ostream& output) {
    output << "nxinspect - inspect Nintendo Switch executable headers\n\n"
              "Usage:\n"
              "  nxinspect [options] <file.nso|file.nro|file.elf>\n\n"
              "Options:\n"
              "  --format <text|json>  Select output format (default: text)\n"
              "  --strict              Treat warnings as a failure exit code\n"
              "  -h, --help            Show this help\n"
              "  --version             Show the tool version\n\n"
              "Exit codes:\n"
              "  0  inspection completed without errors\n"
              "  1  malformed or unsupported input\n"
              "  2  invalid command line\n"
              "  3  input could not be read\n"
              "  4  warnings found while --strict was enabled\n";
}

std::optional<OutputFormat> parse_output_format(std::string_view value) {
    if (value == "text") {
        return OutputFormat::text;
    }
    if (value == "json") {
        return OutputFormat::json;
    }
    return std::nullopt;
}

bool read_file(const std::string& path, std::vector<std::byte>& bytes) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) {
        std::cerr << "nxinspect: cannot open '" << path << "'\n";
        return false;
    }

    const std::streampos end_position = input.tellg();
    const std::streamoff length = static_cast<std::streamoff>(end_position);
    if (length < 0) {
        std::cerr << "nxinspect: cannot determine the size of '" << path << "'\n";
        return false;
    }

    const auto unsigned_length = static_cast<std::uint64_t>(length);
    if (unsigned_length > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        std::cerr << "nxinspect: '" << path << "' is too large for this process\n";
        return false;
    }

    bytes.resize(static_cast<std::size_t>(unsigned_length));
    if (bytes.empty()) {
        return true;
    }

    input.seekg(0, std::ios::beg);
    input.read(reinterpret_cast<char*>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    if (!input) {
        std::cerr << "nxinspect: failed to read the complete contents of '" << path << "'\n";
        return false;
    }

    return true;
}

} // namespace

int main(int argc, char** argv) {
    Options options;

    for (int index = 1; index < argc; ++index) {
        const std::string_view argument = argv[index];

        if (argument == "-h" || argument == "--help") {
            print_usage(std::cout);
            return 0;
        }
        if (argument == "--version") {
            std::cout << "nxinspect " << NXRELINKER_VERSION_STRING << '\n';
            return 0;
        }
        if (argument == "--strict") {
            options.strict = true;
            continue;
        }
        if (argument == "--format") {
            if (index + 1 >= argc) {
                std::cerr << "nxinspect: --format requires a value\n";
                return 2;
            }
            const std::optional<OutputFormat> format = parse_output_format(argv[++index]);
            if (!format) {
                std::cerr << "nxinspect: unknown output format '" << argv[index] << "'\n";
                return 2;
            }
            options.output_format = *format;
            continue;
        }
        if (argument.starts_with("--format=")) {
            const std::optional<OutputFormat> format =
                parse_output_format(argument.substr(std::string_view("--format=").size()));
            if (!format) {
                std::cerr << "nxinspect: unknown output format '"
                          << argument.substr(std::string_view("--format=").size()) << "'\n";
                return 2;
            }
            options.output_format = *format;
            continue;
        }
        if (!argument.empty() && argument.front() == '-') {
            std::cerr << "nxinspect: unknown option '" << argument << "'\n";
            return 2;
        }
        if (!options.path.empty()) {
            std::cerr << "nxinspect: only one input file may be specified\n";
            return 2;
        }
        options.path = std::string(argument);
    }

    if (options.path.empty()) {
        std::cerr << "nxinspect: no input file specified\n\n";
        print_usage(std::cerr);
        return 2;
    }

    std::vector<std::byte> bytes;
    if (!read_file(options.path, bytes)) {
        return 3;
    }

    const nxrelinker::binary::BinaryInspection inspection =
        nxrelinker::binary::inspect_binary(bytes, options.path);

    if (options.output_format == OutputFormat::json) {
        std::cout << nxrelinker::binary::format_json(inspection) << '\n';
    } else {
        std::cout << nxrelinker::binary::format_text(inspection);
    }

    if (inspection.diagnostics.has_errors()) {
        return 1;
    }
    if (options.strict && inspection.diagnostics.has_warnings()) {
        return 4;
    }
    return 0;
}
