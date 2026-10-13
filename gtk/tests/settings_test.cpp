#include "core/settings.h"

#include <glib.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace emojipicker;

namespace {

struct TemporaryConfig {
    std::string folder;
    std::string path;

    TemporaryConfig()
    {
        gchar *created = g_dir_make_tmp("emojipicker-settings-XXXXXX", nullptr);
        g_assert_nonnull(created);
        folder = created;
        g_free(created);
        path = folder + "/lgl-emoji-picker/settings.ini";
    }
    ~TemporaryConfig()
    {
        std::error_code error;
        std::filesystem::remove_all(folder, error);
    }
};

void missingFileUsesDefaults()
{
    TemporaryConfig config;
    g_assert_false(Settings(config.path).systemTitleBar());
}

void systemTitleBarPersists()
{
    TemporaryConfig config;
    Settings(config.path).setSystemTitleBar(true);
    g_assert_true(std::filesystem::exists(config.path));
    g_assert_true(Settings(config.path).systemTitleBar());
    Settings(config.path).setSystemTitleBar(false);
    g_assert_false(Settings(config.path).systemTitleBar());
}

void malformedValueUsesDefault()
{
    TemporaryConfig config;
    std::filesystem::create_directories(config.folder + "/lgl-emoji-picker");
    std::ofstream(config.path) << "[window]\nsystemTitleBar=maybe\n";
    g_assert_false(Settings(config.path).systemTitleBar());
}

} // namespace

int main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/settings/missing-file-uses-defaults", missingFileUsesDefaults);
    g_test_add_func("/settings/system-title-bar-persists", systemTitleBarPersists);
    g_test_add_func("/settings/malformed-value-uses-default", malformedValueUsesDefault);
    return g_test_run();
}
