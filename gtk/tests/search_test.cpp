#include "core/emojidata.h"
#include "core/search.h"

#include <glib.h>

#include <string>

using namespace emojipicker;

namespace {

bool matchesQuery(const char *emoji, const char *name, const std::string &query)
{
    return search::matches(search::fold(name), emoji, search::fold(query));
}

void matchesNamesCaseInsensitively()
{
    g_assert_true(matchesQuery("😀", "grinning face", ""));
    g_assert_true(matchesQuery("😀", "grinning face", "GRIN"));
    g_assert_true(matchesQuery("😀", "grinning face", "face"));
    g_assert_false(matchesQuery("😀", "grinning face", "cat"));
    g_assert_true(matchesQuery("🇦🇨", "flag Ascension Island", "ascension"));
}

void matchesEmojiCharacters()
{
    g_assert_true(matchesQuery("😀", "grinning face", "😀"));
    g_assert_false(matchesQuery("😃", "grinning face with big eyes", "😀"));
}

void looksUpNames()
{
    g_assert_true(search::lookupName("😀") == "grinning face");
    g_assert_true(search::lookupName("not an emoji") == "not an emoji");
}

void catalogIsValid()
{
    for (const Emoji &entry : EMOJIS) {
        g_assert_true(g_utf8_validate(entry.emoji, -1, nullptr));
        g_assert_true(g_utf8_validate(entry.name, -1, nullptr));
        g_assert_cmpuint(std::string(entry.name).size(), >, 0);
    }
    g_assert_cmpuint(std::size(EMOJIS), >, 1000);
}

} // namespace

int main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/search/matches-names-case-insensitively", matchesNamesCaseInsensitively);
    g_test_add_func("/search/matches-emoji-characters", matchesEmojiCharacters);
    g_test_add_func("/search/looks-up-names", looksUpNames);
    g_test_add_func("/search/catalog-is-valid", catalogIsValid);
    return g_test_run();
}
