#include "core/recentemojis.h"

#include <glib.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using namespace emojipicker;

namespace {

struct TemporaryCache {
    std::string folder;
    std::string path;

    TemporaryCache()
    {
        gchar *created = g_dir_make_tmp("emojipicker-recent-XXXXXX", nullptr);
        g_assert_nonnull(created);
        folder = created;
        g_free(created);
        path = folder + "/lgl-emoji-picker/recent.txt";
    }
    ~TemporaryCache()
    {
        std::error_code error;
        std::filesystem::remove_all(folder, error);
    }
    void write(const std::string &text) const
    {
        std::filesystem::create_directories(folder + "/lgl-emoji-picker");
        std::ofstream(path) << text;
    }
    std::string read() const
    {
        std::ifstream file(path);
        std::ostringstream text;
        text << file.rdbuf();
        return text.str();
    }
};

void missingFileIsEmpty()
{
    TemporaryCache cache;
    g_assert_true(recent::load(cache.path).empty());
}

void addCreatesFileAndMovesToFront()
{
    TemporaryCache cache;
    recent::add(cache.path, "😀");
    recent::add(cache.path, "🎉");
    const auto result = recent::add(cache.path, "😀");
    g_assert_cmpuint(result.size(), ==, 2);
    g_assert_true(result[0] == "😀");
    g_assert_true(result[1] == "🎉");
    g_assert_true(cache.read() == "😀\n🎉\n");
}

void capsHistory()
{
    TemporaryCache cache;
    for (int i = 0; i < 20; ++i)
        recent::add(cache.path, std::string(1, static_cast<char>('a' + i)));
    const auto result = recent::load(cache.path);
    g_assert_cmpuint(result.size(), ==, recent::maxRecent);
    g_assert_true(result.front() == "t");
}

void ignoresMalformedLines()
{
    TemporaryCache cache;
    cache.write("  🎉  \n\n" + std::string(40, 'x') + "\n\xff\xfe\n🎉\n👍🏽\n");
    const auto result = recent::load(cache.path);
    g_assert_cmpuint(result.size(), ==, 2);
    g_assert_true(result[0] == "🎉");
    g_assert_true(result[1] == "👍🏽");
}

void readsQtHistory()
{
    // The Qt app wrote the same one-emoji-per-line UTF-8 format, including ZWJ sequences.
    TemporaryCache cache;
    cache.write("❤️\n👨‍👩‍👧‍👦\n");
    const auto result = recent::load(cache.path);
    g_assert_cmpuint(result.size(), ==, 2);
    g_assert_true(result[1] == "👨‍👩‍👧‍👦");
}

} // namespace

int main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/recent/missing-file-is-empty", missingFileIsEmpty);
    g_test_add_func("/recent/add-creates-file-and-moves-to-front", addCreatesFileAndMovesToFront);
    g_test_add_func("/recent/caps-history", capsHistory);
    g_test_add_func("/recent/ignores-malformed-lines", ignoresMalformedLines);
    g_test_add_func("/recent/reads-qt-history", readsQtHistory);
    return g_test_run();
}
