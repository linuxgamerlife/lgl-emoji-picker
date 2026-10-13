#pragma once

#include <cstddef>
#include <string>
#include <vector>

// Recent emoji history, one emoji per line in $XDG_CACHE_HOME/lgl-emoji-picker/recent.txt
// (the same file the Qt app used). Reads are capped and malformed lines ignored; writes are
// atomic.
namespace emojipicker::recent {

constexpr std::size_t maxRecent = 16;

std::string defaultPath();

std::vector<std::string> load(const std::string &path);

// Moves emoji to the front of the history, saves it and returns the new history.
std::vector<std::string> add(const std::string &path, const std::string &emoji);

} // namespace emojipicker::recent
