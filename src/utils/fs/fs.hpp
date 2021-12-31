#pragma once

#include <optional>
#include <string>

namespace utils
{
namespace fs
{
std::optional<std::string> loadFileToString(const char *fpath);
}
} // namespace utils
