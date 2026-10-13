#include "core/search.h"

#include "core/emojidata.h"

#include <glib.h>

namespace emojipicker::search {

std::string fold(const std::string &text)
{
    gchar *folded = g_utf8_casefold(text.c_str(), static_cast<gssize>(text.size()));
    std::string result = folded ? folded : "";
    g_free(folded);
    return result;
}

bool matches(const std::string &foldedName, const std::string &emoji,
             const std::string &foldedQuery)
{
    return foldedQuery.empty() || foldedName.find(foldedQuery) != std::string::npos
        || emoji.find(foldedQuery) != std::string::npos;
}

std::string lookupName(const std::string &emoji)
{
    for (const Emoji &entry : EMOJIS) {
        if (emoji == entry.emoji)
            return entry.name;
    }
    return emoji;
}

} // namespace emojipicker::search
