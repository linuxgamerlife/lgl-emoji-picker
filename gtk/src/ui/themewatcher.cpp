#include "ui/themewatcher.h"

#include "core/theme.h"

#include <adwaita.h>
#include <giomm/file.h>
#include <giomm/settingsschemasource.h>
#include <glibmm/fileutils.h>
#include <glibmm/main.h>
#include <glibmm/miscutils.h>
#include <gtkmm/settings.h>
#include <gtkmm/styleprovider.h>

#include <algorithm>
#include <cctype>
#include <vector>

namespace emojipicker {

namespace {

constexpr unsigned int debounceMs = 200;

bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.size() >= suffix.size()
        && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace

void installApplicationStylesheet()
{
    adw_init();
    auto provider = Gtk::CssProvider::create();
    provider->signal_parsing_error().connect(
        [](const Glib::RefPtr<const Gtk::CssSection> &section, const Glib::Error &error) {
            g_warning("style.css:%u: %s",
                      section ? static_cast<unsigned>(section->get_start_location().get_lines() + 1)
                              : 0u,
                      error.what());
        });
    provider->load_from_resource("/io/github/linuxgamerlife/emojipicker/style.css");
    Gtk::StyleProvider::add_provider_for_display(Gdk::Display::get_default(), provider,
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

ThemeWatcher::ThemeWatcher()
    : m_userProvider(Gtk::CssProvider::create())
{
    // The user's gtk.css commonly @imports files that do not exist (libadwaita.css);
    // GTK already reports those once at startup.
    m_userProvider->signal_parsing_error().connect(
        [](const Glib::RefPtr<const Gtk::CssSection> &, const Glib::Error &) {});
    Gtk::StyleProvider::add_provider_for_display(Gdk::Display::get_default(), m_userProvider,
                                                 GTK_STYLE_PROVIDER_PRIORITY_USER);
    auto source = Gio::SettingsSchemaSource::get_default();
    if (source && source->lookup("org.gnome.desktop.interface", true)) {
        m_interface = Gio::Settings::create("org.gnome.desktop.interface");
        m_interface->signal_changed().connect([this](const Glib::ustring &) { queue(); });
    }
    watchFiles();
    apply(false);
}

void ThemeWatcher::watchFiles()
{
    const std::string config = Glib::get_user_config_dir();
    std::vector<std::string> folders{Glib::build_filename(config, "gtk-4.0")};
    for (const std::string &path : theme::gtkCssPaths(config))
        folders.push_back(Glib::path_get_dirname(path));
    for (const std::string &folder : folders) {
        if (m_monitors.count(folder) != 0)
            continue;
        try {
            auto monitor = Gio::File::create_for_path(folder)->monitor_directory(
                Gio::FileMonitorFlags::WATCH_MOVES);
            monitor->signal_changed().connect(
                [this](const Glib::RefPtr<Gio::File> &file, const Glib::RefPtr<Gio::File> &other,
                       Gio::FileMonitor::Event) {
                    for (const auto &changed : {file, other}) {
                        if (changed && endsWith(changed->get_basename(), ".css")) {
                            queue();
                            return;
                        }
                    }
                });
            m_monitors[folder] = monitor;
        } catch (const Glib::Error &) {
        }
    }
}

void ThemeWatcher::queue()
{
    // Theme generators write several files in quick succession; apply once.
    m_timeout.disconnect();
    m_timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &ThemeWatcher::changed),
                                               debounceMs);
}

bool ThemeWatcher::changed()
{
    watchFiles();
    apply(true);
    return false;
}

std::string ThemeWatcher::interfaceString(const char *key) const
{
    if (!m_interface)
        return {};
    auto schema = m_interface->property_settings_schema().get_value();
    if (!schema || !schema->has_key(key))
        return {};
    return m_interface->get_string(key);
}

void ThemeWatcher::followDesktopWithoutPortal()
{
    // libadwaita takes dark mode from the settings portal. Without one, fall back to GNOME's
    // color-scheme, settings.ini, or a "-dark" theme name.
    AdwStyleManager *manager = adw_style_manager_get_default();
    if (adw_style_manager_get_system_supports_color_schemes(manager))
        return;
    auto settings = Gtk::Settings::get_default();
    std::string themeName = settings->property_gtk_theme_name().get_value();
    std::transform(themeName.begin(), themeName.end(), themeName.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    const bool dark = interfaceString("color-scheme") == "prefer-dark"
        || settings->property_gtk_application_prefer_dark_theme().get_value()
        || endsWith(themeName, "-dark");
    adw_style_manager_set_color_scheme(manager, dark ? ADW_COLOR_SCHEME_FORCE_DARK
                                                     : ADW_COLOR_SCHEME_DEFAULT);
}

void ThemeWatcher::apply(const bool reloadUserCss)
{
    followDesktopWithoutPortal();
    if (!reloadUserCss)
        return;
    const std::string userCss =
        Glib::build_filename(Glib::get_user_config_dir(), "gtk-4.0", "gtk.css");
    if (Glib::file_test(userCss, Glib::FileTest::IS_REGULAR))
        m_userProvider->load_from_path(userCss);
    else
        m_userProvider->load_from_string("");
}

} // namespace emojipicker
