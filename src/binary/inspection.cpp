#include "nxrelinker/binary/inspection.h"

#include "nxrelinker/binary/elf.h"
#include "nxrelinker/binary/nro.h"
#include "nxrelinker/binary/nso.h"
#include "nxrelinker/core/hex.h"
#include "nxrelinker/diagnostics/json_writer.h"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace nxrelinker::binary {

namespace {

std::string hex_value(std::uint64_t value) {
    std::ostringstream output;
    output << "0x" << std::hex << value;
    return output.str();
}

void add_nso_segments(BinaryInspection& inspection, const NsoHeader& header) {
    const auto add_segment = [&inspection](const char* name, const NsoSegment& segment) {
        std::uint64_t physical_size = segment.memory_size;
        if (segment.compressed) {
            physical_size = segment.compressed_file_size;
        }
        inspection.segments.push_back(SegmentInfo{
            name,
            segment.file_offset,
            physical_size,
            segment.memory_offset,
            segment.memory_size,
            segment.compressed,
        });
    };

    add_segment("text", header.text);
    add_segment("ro", header.ro);
    add_segment("data", header.data);
}

void add_nro_segments(BinaryInspection& inspection, const NroHeader& header) {
    const bool compressed = (header.flags & (1U << 1U)) != 0;
    const auto add_segment = [&inspection, compressed](const char* name,
                                                       const NroSegment& segment) {
        inspection.segments.push_back(SegmentInfo{
            name,
            std::nullopt,
            std::nullopt,
            segment.memory_offset,
            segment.size,
            compressed,
        });
    };

    add_segment("text", header.text);
    add_segment("ro", header.ro);
    add_segment("data", header.data);
}

void write_json_header(diagnostics::JsonWriter& writer, const BinaryInspection& inspection) {
    writer.key("schema_version");
    writer.number(1);
    writer.key("path");
    writer.string(inspection.path);
    writer.key("file_size");
    writer.number(inspection.file_size);
    writer.key("format");
    writer.string(format_name(inspection.format));
    writer.key("magic");
    writer.string(inspection.magic);

    writer.key("magic_offset");
    if (inspection.magic_offset) {
        writer.number(*inspection.magic_offset);
    } else {
        writer.null();
    }

    writer.key("version");
    if (inspection.version) {
        writer.number(*inspection.version);
    } else {
        writer.null();
    }

    writer.key("flags");
    if (inspection.flags) {
        writer.number(*inspection.flags);
    } else {
        writer.null();
    }

    writer.key("module_id");
    writer.string(inspection.module_id_hex);

    writer.key("bss_size");
    if (inspection.bss_size) {
        writer.number(*inspection.bss_size);
    } else {
        writer.null();
    }

    writer.key("entry_point");
    if (inspection.entry_point) {
        writer.number(*inspection.entry_point);
    } else {
        writer.null();
    }

    writer.key("elf_machine");
    if (inspection.elf_machine) {
        writer.number(*inspection.elf_machine);
    } else {
        writer.null();
    }
}

void write_json_segments(diagnostics::JsonWriter& writer, const BinaryInspection& inspection) {
    writer.key("segments");
    writer.start_array();
    for (const SegmentInfo& segment : inspection.segments) {
        writer.start_object();
        writer.key("name");
        writer.string(segment.name);
        writer.key("file_offset");
        if (segment.file_offset) {
            writer.number(*segment.file_offset);
        } else {
            writer.null();
        }
        writer.key("file_size");
        if (segment.file_size) {
            writer.number(*segment.file_size);
        } else {
            writer.null();
        }
        writer.key("memory_offset");
        writer.number(segment.memory_offset);
        writer.key("memory_size");
        writer.number(segment.memory_size);
        writer.key("compressed");
        writer.boolean(segment.compressed);
        writer.end_object();
    }
    writer.end_array();
}

void write_json_diagnostics(diagnostics::JsonWriter& writer,
                            const BinaryInspection& inspection) {
    writer.key("diagnostics");
    writer.start_array();
    for (const diagnostics::Diagnostic& diagnostic : inspection.diagnostics.items()) {
        writer.start_object();
        writer.key("severity");
        writer.string(diagnostics::json_name(diagnostic.severity));
        writer.key("code");
        writer.string(diagnostic.code);
        writer.key("message");
        writer.string(diagnostic.message);
        writer.key("offset");
        if (diagnostic.offset) {
            writer.number(*diagnostic.offset);
        } else {
            writer.null();
        }
        writer.end_object();
    }
    writer.end_array();
}

void write_json_summary(diagnostics::JsonWriter& writer, const BinaryInspection& inspection) {
    writer.key("summary");
    writer.start_object();
    writer.key("errors");
    writer.number(inspection.diagnostics.count(diagnostics::Severity::error));
    writer.key("warnings");
    writer.number(inspection.diagnostics.count(diagnostics::Severity::warning));
    writer.key("info");
    writer.number(inspection.diagnostics.count(diagnostics::Severity::info));
    writer.end_object();
}

} // namespace

BinaryInspection inspect_binary(ByteSpan data, std::string_view path) {
    BinaryInspection inspection;
    inspection.path = std::string(path);
    inspection.file_size = data.size();

    const FormatDetection detection = detect_format(data);
    inspection.format = detection.format;
    inspection.magic = detection.magic;
    inspection.magic_offset = detection.magic_offset;

    switch (detection.format) {
    case BinaryFormat::nso: {
        ParseResult<NsoHeader> parsed = parse_nso(data);
        inspection.diagnostics = std::move(parsed.diagnostics);
        if (!parsed) {
            break;
        }
        const NsoHeader& header = *parsed;
        inspection.version = header.version;
        inspection.flags = header.flags;
        inspection.bss_size = header.bss_size;
        inspection.module_id_hex = to_hex(header.module_id);
        add_nso_segments(inspection, header);
        break;
    }
    case BinaryFormat::nro: {
        ParseResult<NroHeader> parsed = parse_nro(data);
        inspection.diagnostics = std::move(parsed.diagnostics);
        if (!parsed) {
            break;
        }
        const NroHeader& header = *parsed;
        inspection.version = header.version;
        inspection.flags = header.flags;
        inspection.bss_size = header.bss_size;
        inspection.module_id_hex = to_hex(header.module_id);
        add_nro_segments(inspection, header);
        break;
    }
    case BinaryFormat::elf64: {
        ParseResult<Elf64Header> parsed = parse_elf64(data);
        inspection.diagnostics = std::move(parsed.diagnostics);
        if (!parsed) {
            break;
        }
        const Elf64Header& header = *parsed;
        inspection.entry_point = header.entry_point;
        inspection.elf_machine = header.machine;
        break;
    }
    case BinaryFormat::unknown:
        inspection.diagnostics.error(
            "format.unknown",
            "input does not match a supported signature (NSO0 at 0x0, NRO0 at 0x10, ELF at 0x0)",
            0);
        break;
    }

    return inspection;
}

std::string format_text(const BinaryInspection& inspection) {
    std::ostringstream output;
    output << "File: " << (inspection.path.empty() ? "<memory>" : inspection.path) << '\n';
    output << "Format: " << format_name(inspection.format) << '\n';
    output << "File size: " << inspection.file_size << " bytes ("
           << hex_value(inspection.file_size) << ")\n";

    if (inspection.magic_offset) {
        output << "Signature offset: " << hex_value(*inspection.magic_offset) << '\n';
    }
    if (inspection.version) {
        output << "Version: " << *inspection.version << '\n';
    }
    if (inspection.flags) {
        output << "Flags: " << hex_value(*inspection.flags) << '\n';
    }
    if (!inspection.module_id_hex.empty()) {
        output << "Module ID: " << inspection.module_id_hex << '\n';
    }
    if (inspection.bss_size) {
        output << "BSS size: " << *inspection.bss_size << " bytes ("
               << hex_value(*inspection.bss_size) << ")\n";
    }
    if (inspection.entry_point) {
        output << "Entry point: " << hex_value(*inspection.entry_point) << '\n';
    }
    if (inspection.elf_machine) {
        output << "ELF machine: " << *inspection.elf_machine << '\n';
    }

    if (!inspection.segments.empty()) {
        output << "Segments:\n";
        for (const SegmentInfo& segment : inspection.segments) {
            output << "  " << segment.name << ": memory=" << hex_value(segment.memory_offset)
                   << " memory_size=" << segment.memory_size;
            if (segment.file_offset) {
                output << " file=" << hex_value(*segment.file_offset);
            }
            if (segment.file_size) {
                output << " file_size=" << *segment.file_size;
            }
            if (segment.compressed) {
                output << " compressed";
            }
            output << '\n';
        }
    }

    const std::size_t errors =
        inspection.diagnostics.count(diagnostics::Severity::error);
    const std::size_t warnings =
        inspection.diagnostics.count(diagnostics::Severity::warning);
    const std::size_t info = inspection.diagnostics.count(diagnostics::Severity::info);
    output << "Diagnostics: " << errors << " error(s), " << warnings << " warning(s), " << info
           << " info\n";

    for (const diagnostics::Diagnostic& diagnostic : inspection.diagnostics.items()) {
        output << "  [" << diagnostics::to_string(diagnostic.severity) << "] "
               << diagnostic.code;
        if (diagnostic.offset) {
            output << " at " << hex_value(*diagnostic.offset);
        }
        output << ": " << diagnostic.message << '\n';
    }

    return output.str();
}

std::string format_json(const BinaryInspection& inspection) {
    diagnostics::JsonWriter writer;
    writer.start_object();
    write_json_header(writer, inspection);
    write_json_segments(writer, inspection);
    write_json_diagnostics(writer, inspection);
    write_json_summary(writer, inspection);
    writer.end_object();
    return writer.finish();
}

} // namespace nxrelinker::binary
