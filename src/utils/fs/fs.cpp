#include <utils/fs/fs.hpp>

#include <fstream>

namespace utils::fs
{
std::optional<std::string> loadFileToString(const char *fpath)
{
    std::optional<std::string> content;
    std::ifstream file(fpath);

    if (file.is_open())
    {
        content = std::string(std::istreambuf_iterator<char>(file), {});
    }

    return content;
}
} // namespace utils::fs
