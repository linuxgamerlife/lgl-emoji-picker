#pragma once

#include <string>

namespace emojipicker::search {

// Case-folds UTF-8 text for comparison.
std::string fold(const std::string &text);

// True when the folded query is empty, appears in the folded name, or appears in the
// emoji itself (so pasting an emoji finds it).
bool matches(const std::string &foldedName, const std::string &emoji,
             const std::string &foldedQuery);

// The catalog name of an emoji, or the emoji itself when it is not in the catalog.
std::string lookupName(const std::string &emoji);

} // namespace emojipicker::search
