#include "core/clipboardhelper.h"

#include <glib.h>

using namespace emojipicker;

namespace {

void choosesHelpersForSession()
{
    const auto wayland = clipboard::helperCommands(true, true);
    g_assert_cmpuint(wayland.size(), ==, 2);
    g_assert_true(wayland[0].front() == "/usr/bin/wl-copy");

    const auto x11 = clipboard::helperCommands(false, true);
    g_assert_cmpuint(x11.size(), ==, 2);
    g_assert_true(x11[0].front() == "/usr/bin/xclip");
    g_assert_true(x11[1].front() == "/usr/bin/xsel");

    g_assert_true(clipboard::helperCommands(false, false).empty());
}

void helpersUseAbsolutePaths()
{
    for (const bool wayland : {true, false}) {
        for (const auto &command : clipboard::helperCommands(wayland, !wayland))
            g_assert_true(g_path_is_absolute(command.front().c_str()));
    }
}

} // namespace

int main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, nullptr);
    g_test_add_func("/clipboard/chooses-helpers-for-session", choosesHelpersForSession);
    g_test_add_func("/clipboard/helpers-use-absolute-paths", helpersUseAbsolutePaths);
    return g_test_run();
}
