#include <filesystem>
#include <string_view>

namespace util
{
    /// assets path is modified from ".../<chapter_name>/assets" to "<program_path>/../assets/<chapter_name>"
    std::filesystem::path assets_path(std::string_view chapter_name);
}
