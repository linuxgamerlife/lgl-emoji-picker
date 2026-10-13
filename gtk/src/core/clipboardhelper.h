#pragma once

#include <string>
#include <vector>

// Command-line clipboard helpers, used after GDK has taken the clipboard so the emoji
// survives in compositors that drop a selection when its owner loses focus. Helpers run with
// fixed absolute paths and argv (never a shell); the emoji is written to stdin.
namespace emojipicker::clipboard {

using Command = std::vector<std::string>;

// The helper commands to try, in order, for the session type.
std::vector<Command> helperCommands(bool wayland, bool x11);

// Tries each installed helper for the current session until one succeeds. Asynchronous; runs
// on the thread-default main context.
void copyWithHelpers(const std::string &text);

} // namespace emojipicker::clipboard
