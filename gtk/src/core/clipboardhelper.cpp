#include "core/clipboardhelper.h"

#include <glibmm/bytes.h>
#include <glibmm/fileutils.h>
#include <glibmm/main.h>
#include <glibmm/miscutils.h>
#include <giomm/subprocess.h>

#include <memory>

namespace emojipicker::clipboard {

namespace {

constexpr unsigned int helperTimeoutMs = 2000;

void tryNext(std::shared_ptr<std::vector<Command>> commands, std::size_t index,
             std::shared_ptr<const std::string> text)
{
    while (index < commands->size()
           && !Glib::file_test((*commands)[index].front(), Glib::FileTest::IS_EXECUTABLE))
        ++index;
    if (index >= commands->size())
        return;

    Glib::RefPtr<Gio::Subprocess> process;
    try {
        process = Gio::Subprocess::create((*commands)[index],
                                          Gio::Subprocess::Flags::STDIN_PIPE
                                              | Gio::Subprocess::Flags::STDOUT_SILENCE
                                              | Gio::Subprocess::Flags::STDERR_SILENCE);
    } catch (const Glib::Error &) {
        tryNext(commands, index + 1, text);
        return;
    }

    auto timedOut = std::make_shared<bool>(false);
    auto timeout = Glib::TimeoutSource::create(helperTimeoutMs);
    timeout->connect([process, timedOut] {
        *timedOut = true;
        process->force_exit();
        return false;
    });
    timeout->attach(Glib::MainContext::get_thread_default());

    process->communicate_async(
        Glib::Bytes::create(text->data(), text->size()),
        [process, timedOut, timeout, commands, index, text](
            Glib::RefPtr<Gio::AsyncResult> &result) {
            timeout->destroy();
            bool succeeded = false;
            try {
                process->communicate_finish(result);
                succeeded = !*timedOut && process->get_successful();
            } catch (const Glib::Error &) {
            }
            if (!succeeded)
                tryNext(commands, index + 1, text);
        });
}

} // namespace

std::vector<Command> helperCommands(const bool wayland, const bool x11)
{
    if (wayland)
        return {{"/usr/bin/wl-copy", "--type", "text/plain;charset=utf-8"}, {"/usr/bin/wl-copy"}};
    if (x11)
        return {{"/usr/bin/xclip", "-selection", "clipboard"},
                {"/usr/bin/xsel", "--clipboard", "--input"}};
    return {};
}

void copyWithHelpers(const std::string &text)
{
    auto commands = std::make_shared<std::vector<Command>>(helperCommands(
        !Glib::getenv("WAYLAND_DISPLAY").empty(), !Glib::getenv("DISPLAY").empty()));
    tryNext(commands, 0, std::make_shared<const std::string>(text));
}

} // namespace emojipicker::clipboard
