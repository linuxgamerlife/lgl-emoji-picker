#pragma once

#include <string>
#include <vector>

// Port of lgl-papercutter's core/theme: finds the desktop's GTK 4 stylesheet
// (~/.config/gtk-4.0/gtk.css, as written by generators such as Noctalia) and the local files
// it imports, so they can be watched. Pure functions so they can be tested without a display.
namespace emojipicker::theme {

// The user GTK 4 stylesheet and every local file it imports, in load order.
std::vector<std::string> gtkCssPaths(const std::string &configDir);

} // namespace emojipicker::theme
