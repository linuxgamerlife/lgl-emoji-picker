#pragma once

#include <gtkmm/aboutdialog.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>

#include <memory>
#include <string>
#include <vector>

namespace emojipicker {

class EmojiTile;
class ThemeWatcher;

// The picker window. Port of the Qt EmojiPicker dialog: search, recent history, emoji grid
// and keyboard navigation. Selecting an emoji copies it and keeps the window open.
class Window final : public Gtk::ApplicationWindow {
public:
    explicit Window(const Glib::RefPtr<Gtk::Application> &application);
    ~Window() override;

private:
    void buildHeader();
    void buildBody();
    void populateGrid();
    void populateRecent();
    void onSearchChanged();
    void selectEmoji(const std::string &emoji);
    void showAbout();

    bool onKeyPressed(guint keyval, Gdk::ModifierType state);
    bool handleTileKey(EmojiTile &tile, guint keyval);
    void focusTile(const std::vector<EmojiTile *> &tiles, int index);
    void focusSearch();
    bool searchHasFocus();
    bool recentVisible() const;

    std::string m_recentPath;
    std::vector<std::string> m_recentEmojis;
    std::string m_query;

    Gtk::Box m_root;
    Gtk::SearchEntry m_search;
    Gtk::Box m_recentBox;
    Gtk::FlowBox m_recent;
    Gtk::ScrolledWindow m_scroller;
    Gtk::FlowBox m_grid;

    std::vector<EmojiTile *> m_recentTiles;
    std::vector<EmojiTile *> m_allTiles;
    std::vector<EmojiTile *> m_visibleTiles;

    Gtk::MenuButton *m_menuButton{nullptr};
    std::unique_ptr<ThemeWatcher> m_theme;
    std::unique_ptr<Gtk::AboutDialog> m_about;
};

} // namespace emojipicker
