#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <util/path.h>

const char* path::get_home() { return getenv("HOME"); }

bool path::make_abs(char* out, int size_out, const char* base, const char* rel)
{
    const char char_separator = '/';
    const char* const separator = "/";

    if (base == nullptr ||
        rel == nullptr)
    {
        return false;
    }

    int n_base = strlen(base);
    int n_rel = strlen(rel);
    bool has_separator_base = base[n_base - 1] == char_separator;
    bool has_separator_rel = rel[0] == char_separator;
    if (has_separator_rel)
    {
        ++rel;
        --n_rel;
    }

    int n = n_base + n_rel + (has_separator_base ? 0 : 1) + 1;
    if (n > size_out ||
        n > PATH_MAX)
    {
        return false;
        }

    return snprintf(out, size_out, "%s%s%s", base,
                    has_separator_base ? "" : separator, rel) > 0;
}

int path::file_exists(const char* path)
{
    struct stat s;
    int err = stat(path, &s);
    if (err == 0)
    {
        if (S_ISDIR(s.st_mode))
        {
            return EISDIR;
        }
        else if (S_ISREG(s.st_mode))
        {
            return 0;
        }
    }
    else
    {
        return errno;
    }
    return -1;
}

int path::dir_exists(const char* path)
{
    struct stat s;
    int err = stat(path, &s);
    if (err == 0)
    {
        if (S_ISDIR(s.st_mode))
        {
            return 0;
        }
        else
        {
            return ENOTDIR;
        }
    }
    else
    {
        return errno;
    }
    return -1;
}
