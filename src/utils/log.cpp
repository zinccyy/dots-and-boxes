#include <utils/log.hpp>

// std
#include <cstdio>
#include <cstdarg>

namespace utils::log
{
void debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[1;33m[DEBUG]:\033[0m ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
void info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[1;32m[INFO]:\033[0m ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
void error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[1;31m[ERROR]:\033[0m ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
} // namespace utils::log
