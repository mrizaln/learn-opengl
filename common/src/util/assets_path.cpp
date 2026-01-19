#include "common/util/assets_path.hpp"

#include <filesystem>

namespace
{
    std::filesystem::path to_base_assets_path(const char* program_path);

#if defined(_MSC_VER)
    const char* program_path = const_cast<const char*>(__argv[0]);
#else
    // the order of __constructor__ and static initialization is unspecified, but from my testing, static
    // initialization, for a pointer at least, is before __constructor__ called.
    // see: https://stackoverflow.com/q/8433484.
    const char* program_path = nullptr;
#endif

    std::filesystem::path to_base_assets_path(const char* prog)
    {
        auto canonical = std::filesystem::weakly_canonical(prog);
        return std::filesystem::path{ canonical }.parent_path() / "assets";
    }
}

namespace util
{
    /// assets path is modified from ".../<chapter_name>/assets" to "<program_path>/../assets/<chapter_name>"
    std::filesystem::path assets_path(std::string_view chapter_name)
    {
        static auto base_assets_path = to_base_assets_path(program_path);
        return base_assets_path / chapter_name;
    }
}

// https://stackoverflow.com/a/46331112
#if (defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)) && !defined(__EMSCRIPTEN__)
__attribute__((constructor(101))) static void cmd_line_args(int /* argc */, const char** argv)
{
    program_path = argv[0];
}
#else
// For MSVC, largc/largv are initialized with __argc/__argv
#endif
