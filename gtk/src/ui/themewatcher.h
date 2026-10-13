#pragma once

#include <giomm/filemonitor.h>
#include <giomm/settings.h>
#include <gtkmm/cssprovider.h>
#include <sigc++/sigc++.h>

#include <map>
#include <string>

namespace emojipicker {

// Re-themes the app live when the desktop theme changes. Port of lgl-papercutter's
// ThemeWatcher.
//
// libadwaita provides the base stylesheet, follows the desktop's dark mode and accent
// colour (settings portal), and takes its colours from the named colours in the user's
// ~/.config/gtk-4.0/gtk.css (Noctalia and similar generators). This watcher adds what
// libadwaita does not do: it re-reads that gtk.css when it or one of its @imports changes
// (GTK loads it only once), and follows dark mode without a settings portal. Changes are
// debounced by 200 ms.
class ThemeWatcher final : public sigc::trackable {
public:
    ThemeWatcher();
    ThemeWatcher(const ThemeWatcher &) = delete;
    ThemeWatcher &operator=(const ThemeWatcher &) = delete;

private:
    void watchFiles();
    void queue();
    bool changed();
    void followDesktopWithoutPortal();
    void apply(bool reloadUserCss);
    std::string interfaceString(const char *key) const;

    Glib::RefPtr<Gtk::CssProvider> m_userProvider;
    Glib::RefPtr<Gio::Settings> m_interface;
    std::map<std::string, Glib::RefPtr<Gio::FileMonitor>> m_monitors;
    sigc::connection m_timeout;
};

// Initialises libadwaita and adds data/style.css at APPLICATION priority for the default
// display. Call once from the application's startup handler.
void installApplicationStylesheet();

} // namespace emojipicker
