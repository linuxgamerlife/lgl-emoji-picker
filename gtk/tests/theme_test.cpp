#include "core/theme.h"

#include <glib.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace emojipicker;

namespace {

struct TemporaryConfig {
    std::string path;

    TemporaryConfig()
    {
        gchar *created = g_dir_make_tmp("emojipicker-theme-XXXXXX", nullptr);
        g_assert_nonnull(created);
        path = created;
        g_free(created);
        std::filesystem::create_directories(path + "/gtk-4.0");
    }
    ~TemporaryConfig()
    {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
    void write(const std::string &name, const std::string &text) const
    {
        std::ofstream(path + "/gtk-4.0/" + name) << text;
    }
};

void followsImports()
{
    TemporaryConfig config;
    config.write("gtk.css",
                 "/* @import url('ignored.css'); */\n"
                 "@import url('palette.css');\n"
                 "@import \"https://example.com/remote.css\";\n"
                 "@import url(\"missing.css\");\n");
    config.write("palette.css", "@import 'gtk.css';\n@define-color window_bg_color #000000;\n");
    config.write("ignored.css", "");
    const auto paths = theme::gtkCssPaths(config.path);
    g_assert_cmpuint(paths.size(), ==, 2);
    g_assert_true(paths[0].find("gtk.css") != std::string::npos);
    g_assert_true(paths[1].find("palette.css") != std::string::npos);
}

void handlesMissingStylesheet()
{
    TemporaryConfig config;
    g_assert_true(theme::gtkCssPaths(config.path).empty());
}

} // namespace

int main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/theme/follows-imports", followsImports);
    g_test_add_func("/theme/handles-missing-stylesheet", handlesMissingStylesheet);
    return g_test_run();
}
