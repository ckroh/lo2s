#include <lo2s/tracepoint/event_format.hpp>

#include <lo2s/log.hpp>

#include <regex>
#include <string>

#include <boost/algorithm/string/predicate.hpp>

namespace lo2s
{

event_format::event_format(const std::string& name) : name_(name)
{
    boost::filesystem::path path_event = base_path_ / name;
    boost::filesystem::ifstream ifs_id, ifs_format;

    ifs_id.exceptions(std::ios::failbit | std::ios::badbit);
    ifs_format.exceptions(std::ios::failbit | std::ios::badbit);

    try
    {
        ifs_id.open(path_event / "id");
        ifs_id >> id_;

        ifs_format.open(path_event / "format");
        std::string line;
        ifs_format.exceptions(std::ios::badbit);
        while (getline(ifs_format, line))
        {
            parse_format_line(line);
        }
    }
    catch (...)
    {
        log::error() << "Couldn't read information from " << path_event;
        throw;
    }
}

void event_format::parse_format_line(const std::string& line)
{
    static std::regex field_regex(
        "^\\s+field:([^;]+);\\s+offset:(\\d+);\\s+size:(\\d+);\\s+signed:(\\d+);$");
    static std::regex type_name_regex("^(.*) ([^ \\[\\]]+)(\\[[^\\]]+\\])?$");

    std::smatch field_match;
    if (!std::regex_match(line, field_match, field_regex))
    {
        log::trace() << "Discarding line from parsing " << name_ << "/format: " << line;
        return;
    }

    std::string param = field_match[1];
    auto offset = stol(field_match[2]);
    auto size = stol(field_match[3]);

    std::smatch type_name_match;
    if (!std::regex_match(param, type_name_match, type_name_regex))
    {
        log::warn() << "Could not parse type/name of tracepoint event field line for " << name_
                    << ", " << line;
        return;
    }

    std::string name = type_name_match[2];
    event_field field(name, offset, size);

    if (boost::starts_with(name, "common_"))
    {
        common_fields_.emplace_back(field);
    }
    else
    {
        fields_.emplace_back(field);
    }
}

const boost::filesystem::path event_format::base_path_ = "/sys/kernel/debug/tracing/events";
}
