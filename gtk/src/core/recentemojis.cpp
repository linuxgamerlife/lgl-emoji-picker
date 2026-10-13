#include "core/recentemojis.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>

#include <algorithm>
#include <sstream>

namespace emojipicker::recent {

namespace {

// Longer lines are not emoji (the longest catalog entries are ZWJ sequences of ~10 characters).
constexpr glong maxCharacters = 32;

std::string trimmed(const std::string &text)
{
    const char *whitespace = " \t\r\n\f\v";
    const auto first = text.find_first_not_of(whitespace);
    if (first == std::string::npos)
        return {};
    return text.substr(first, text.find_last_not_of(whitespace) - first + 1);
}

bool acceptable(const std::string &line)
{
    return !line.empty() && g_utf8_validate(line.c_str(), static_cast<gssize>(line.size()), nullptr)
        && g_utf8_strlen(line.c_str(), static_cast<gssize>(line.size())) <= maxCharacters;
}

} // namespace

std::string defaultPath()
{
    return Glib::build_filename(Glib::get_user_cache_dir(), "lgl-emoji-picker", "recent.txt");
}

std::vector<std::string> load(const std::string &path)
{
    std::string contents;
    try {
        contents = Glib::file_get_contents(path);
    } catch (const Glib::Error &) {
        return {};
    }
    std::vector<std::string> result;
    std::istringstream lines(contents);
    std::string line;
    while (result.size() < maxRecent && std::getline(lines, line)) {
        line = trimmed(line);
        if (acceptable(line) && std::find(result.begin(), result.end(), line) == result.end())
            result.push_back(line);
    }
    return result;
}

std::vector<std::string> add(const std::string &path, const std::string &emoji)
{
    std::vector<std::string> recent = load(path);
    if (!acceptable(emoji))
        return recent;
    recent.erase(std::remove(recent.begin(), recent.end(), emoji), recent.end());
    recent.insert(recent.begin(), emoji);
    if (recent.size() > maxRecent)
        recent.resize(maxRecent);

    std::string contents;
    for (const std::string &entry : recent)
        contents += entry + '\n';
    g_mkdir_with_parents(Glib::path_get_dirname(path).c_str(), 0700);
    try {
        Glib::file_set_contents(path, contents);
    } catch (const Glib::Error &) {
        // History is a convenience; failing to persist it is not fatal.
    }
    return recent;
}

} // namespace emojipicker::recent
