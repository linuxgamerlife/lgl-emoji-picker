#include "ui/window.h"

#include "core/clipboardhelper.h"
#include "core/emojidata.h"
#include "core/recentemojis.h"
#include "core/search.h"
#include "ui/themewatcher.h"

#include <gdk/gdkkeysyms.h>
#include <gdkmm/clipboard.h>
#include <gdkmm/display.h>
#include <gdkmm/texture.h>
#include <giomm/menu.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/flowboxchild.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/menubutton.h>

#include <algorithm>
#include <cmath>

namespace emojipicker {

namespace {

constexpr int tileSize = 56;
constexpr int tileSpacing = 4;
constexpr unsigned int searchDelayMs = 50;

Glib::RefPtr<Gdk::Texture> creditImage()
{
    static const auto image = Gdk::Texture::create_from_resource(
        "/io/github/linuxgamerlife/emojipicker/Black-Don.png");
    return image;
}

Glib::RefPtr<Gdk::Texture> appIcon()
{
    static const auto icon = Gdk::Texture::create_from_resource(
        "/io/github/linuxgamerlife/emojipicker/lgl-emoji-picker.png");
    return icon;
}

void configureGrid(Gtk::FlowBox &grid)
{
    grid.set_selection_mode(Gtk::SelectionMode::NONE);
    grid.set_homogeneous(true);
    grid.set_min_children_per_line(1);
    grid.set_max_children_per_line(256);
    grid.set_row_spacing(tileSpacing);
    grid.set_column_spacing(tileSpacing);
    grid.set_valign(Gtk::Align::START);
    grid.add_css_class("emoji-grid");
}

} // namespace

// One emoji in a grid. The FlowBox child itself takes focus and is activated by click,
// Space or Enter.
class EmojiTile final : public Gtk::FlowBoxChild {
public:
    EmojiTile(std::string emoji, const std::string &name)
        : m_emoji(std::move(emoji))
        , m_foldedName(search::fold(name))
        , m_label(m_emoji)
    {
        set_size_request(tileSize, tileSize);
        set_halign(Gtk::Align::CENTER);
        set_valign(Gtk::Align::CENTER);
        set_tooltip_text(name);
        add_css_class("emoji-tile");
        set_child(m_label);
    }

    const std::string &emoji() const { return m_emoji; }
    const std::string &foldedName() const { return m_foldedName; }

private:
    std::string m_emoji;
    std::string m_foldedName;
    Gtk::Label m_label;
};

namespace {

// Number of tiles on the first line of a laid-out grid.
int columnCount(const std::vector<EmojiTile *> &tiles)
{
    if (tiles.empty())
        return 1;
    const int firstRow = tiles.front()->get_allocation().get_y();
    int columns = 0;
    for (const EmojiTile *tile : tiles) {
        if (tile->get_allocation().get_y() != firstRow)
            break;
        ++columns;
    }
    return std::max(1, columns);
}

// Horizontal centre of a widget in window coordinates.
double centreX(Gtk::Widget &widget, Gtk::Widget &window)
{
    const graphene_point_t centre =
        GRAPHENE_POINT_INIT(static_cast<float>(widget.get_width()) / 2.0f, 0.0f);
    graphene_point_t translated;
    if (!gtk_widget_compute_point(widget.gobj(), window.gobj(), &centre, &translated))
        return 0.0;
    return translated.x;
}

// The tile in tiles[first, last] closest to x. The two grids are laid out independently
// (Recent is centred), so moving between them goes by position rather than column.
int nearestTile(const std::vector<EmojiTile *> &tiles, const int first, const int last,
                const double x, Gtk::Widget &window)
{
    int nearest = first;
    double best = -1.0;
    for (int index = first; index <= last; ++index) {
        const double distance =
            std::abs(centreX(*tiles[static_cast<std::size_t>(index)], window) - x);
        if (best < 0.0 || distance < best) {
            best = distance;
            nearest = index;
        }
    }
    return nearest;
}

} // namespace

Window::Window(const Glib::RefPtr<Gtk::Application> &application)
    : m_recentPath(recent::defaultPath())
    , m_root(Gtk::Orientation::VERTICAL, 12)
    , m_recentBox(Gtk::Orientation::VERTICAL, 6)
{
    set_application(application);
    set_title("LGL Emoji Picker");
    set_icon_name("lgl-emoji-picker");
    set_default_size(720, 600);

    m_theme = std::make_unique<ThemeWatcher>();
    buildHeader();
    buildBody();
    add_action("about", sigc::mem_fun(*this, &Window::showAbout));

    auto keys = Gtk::EventControllerKey::create();
    keys->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
    keys->signal_key_pressed().connect(
        [this](guint keyval, guint, Gdk::ModifierType state) {
            return onKeyPressed(keyval, state);
        },
        false);
    add_controller(keys);

    m_recentEmojis = recent::load(m_recentPath);
    populateRecent();
    populateGrid();
    set_focus(m_search);
}

Window::~Window() = default;

void Window::buildHeader()
{
    auto *header = Gtk::make_managed<Gtk::HeaderBar>();

    auto menu = Gio::Menu::create();
    auto about = Gio::Menu::create();
    about->append("About LGL Emoji Picker", "win.about");
    menu->append_section(about);
    auto quit = Gio::Menu::create();
    quit->append("Quit", "app.quit");
    menu->append_section(quit);
    m_menuButton = Gtk::make_managed<Gtk::MenuButton>();
    m_menuButton->set_icon_name("open-menu-symbolic");
    m_menuButton->set_menu_model(menu);
    m_menuButton->set_tooltip_text("Main menu");
    m_menuButton->set_primary(true);
    header->pack_end(*m_menuButton);

    set_titlebar(*header);
}

void Window::showAbout()
{
    if (!m_about) {
        m_about = std::make_unique<Gtk::AboutDialog>();
        m_about->set_transient_for(*this);
        m_about->set_modal(true);
        m_about->set_hide_on_close(true);
        m_about->set_program_name("LGL Emoji Picker");
        m_about->set_version(EMOJIPICKER_VERSION);
        m_about->set_comments(
            "Search for an emoji by name or character and copy it to the clipboard. The "
            "picker stays open for repeated use and keeps your recent emojis at the top.\n\n"
            "Built with GTK 4.");
        m_about->set_website("https://github.com/linuxgamerlife/lgl-emoji-picker");
        m_about->set_website_label("Project page and issue tracker");
        m_about->set_authors({"LinuxGamerLife https://www.youtube.com/@linuxgamerlife"});
        m_about->add_credit_section(
            "Inspired by",
            {"TheBlackDon's emoji picker https://codeberg.org/theblackdon/mango-waybar"});
        m_about->add_credit_section("Support",
                                    {"Support development on Ko-fi https://ko-fi.com/G2G3V70LW"});
        m_about->set_license_type(Gtk::License::MIT_X11);
        m_about->set_logo(appIcon());
    }
    m_about->present();
}

void Window::buildBody()
{
    m_root.add_css_class("body");

    m_search.set_placeholder_text("Search emoji...");
    m_search.set_search_delay(searchDelayMs);
    m_search.set_key_capture_widget(*this);
    m_search.signal_search_changed().connect(sigc::mem_fun(*this, &Window::onSearchChanged));
    m_search.signal_activate().connect([this] {
        if (!m_visibleTiles.empty())
            selectEmoji(m_visibleTiles.front()->emoji());
    });
    m_root.append(m_search);

    configureGrid(m_recent);
    m_recent.set_halign(Gtk::Align::CENTER);
    m_recentBox.append(m_recent);
    m_recentBox.add_css_class("ep-card");
    m_root.append(m_recentBox);

    configureGrid(m_grid);
    m_grid.set_filter_func([this](Gtk::FlowBoxChild *child) {
        const auto *tile = static_cast<EmojiTile *>(child);
        return search::matches(tile->foldedName(), tile->emoji(), m_query);
    });
    m_scroller.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    m_scroller.set_vexpand(true);
    m_scroller.set_child(m_grid);
    // The card's rounded corners clip the scrolled grid.
    m_scroller.set_overflow(Gtk::Overflow::HIDDEN);
    m_scroller.add_css_class("ep-card");
    m_scroller.add_css_class("flush");
    m_scroller.add_css_class("emoji-scroller");
    m_root.append(m_scroller);

    for (Gtk::FlowBox *grid : {&m_recent, &m_grid})
        grid->signal_child_activated().connect([this](Gtk::FlowBoxChild *child) {
            selectEmoji(static_cast<EmojiTile *>(child)->emoji());
        });

    auto *credit = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    credit->set_halign(Gtk::Align::END);
    auto *creditIcon = Gtk::make_managed<Gtk::Image>(creditImage());
    creditIcon->set_pixel_size(22);
    credit->append(*creditIcon);
    auto *creditLabel = Gtk::make_managed<Gtk::Label>("Inspired by TheBlackDon");
    creditLabel->add_css_class("credit");
    credit->append(*creditLabel);
    m_root.append(*credit);

    set_child(m_root);
}

void Window::populateGrid()
{
    m_allTiles.reserve(std::size(EMOJIS));
    for (const Emoji &entry : EMOJIS) {
        auto *tile = Gtk::make_managed<EmojiTile>(entry.emoji, entry.name);
        m_grid.append(*tile);
        m_allTiles.push_back(tile);
    }
    m_visibleTiles = m_allTiles;
}

void Window::populateRecent()
{
    m_recent.remove_all();
    m_recentTiles.clear();
    for (const std::string &emoji : m_recentEmojis) {
        auto *tile = Gtk::make_managed<EmojiTile>(emoji, search::lookupName(emoji));
        tile->add_css_class("recent");
        m_recent.append(*tile);
        m_recentTiles.push_back(tile);
    }
    // Size the row to its tiles so the centred grid does not stretch to the window width.
    m_recent.set_max_children_per_line(
        static_cast<guint>(std::max<std::size_t>(1, m_recentTiles.size())));
    m_recentBox.set_visible(!m_recentTiles.empty() && m_query.empty());
}

void Window::onSearchChanged()
{
    m_query = search::fold(m_search.get_text());
    m_visibleTiles.clear();
    for (EmojiTile *tile : m_allTiles) {
        if (search::matches(tile->foldedName(), tile->emoji(), m_query))
            m_visibleTiles.push_back(tile);
    }
    m_grid.invalidate_filter();
    m_recentBox.set_visible(!m_recentTiles.empty() && m_query.empty());
    m_scroller.get_vadjustment()->set_value(0.0);
}

void Window::selectEmoji(const std::string &emoji)
{
    // GDK owns the clipboard while the picker is open; the helpers keep the emoji available
    // after it closes.
    get_display()->get_clipboard()->set_text(emoji);
    get_display()->get_primary_clipboard()->set_text(emoji);
    clipboard::copyWithHelpers(emoji);

    m_recentEmojis = recent::add(m_recentPath, emoji);
    populateRecent();
    focusSearch();
}

bool Window::recentVisible() const
{
    return m_recentBox.get_visible() && !m_recentTiles.empty();
}

bool Window::searchHasFocus()
{
    Gtk::Widget *focus = get_focus();
    return focus && (focus == &m_search || focus->is_ancestor(m_search));
}

void Window::focusSearch()
{
    m_search.grab_focus();
}

void Window::focusTile(const std::vector<EmojiTile *> &tiles, const int index)
{
    if (index >= 0 && index < static_cast<int>(tiles.size()))
        tiles[static_cast<std::size_t>(index)]->grab_focus();
}

bool Window::onKeyPressed(const guint keyval, const Gdk::ModifierType state)
{
    // Escape closes the picker, unless it is closing the main menu.
    if (keyval == GDK_KEY_Escape && !m_menuButton->get_active()) {
        close();
        return true;
    }
    if ((state & (Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::ALT_MASK))
        != Gdk::ModifierType{})
        return false;

    if (searchHasFocus()) {
        if (keyval != GDK_KEY_Down)
            return false;
        if (recentVisible())
            focusTile(m_recentTiles, 0);
        else if (!m_visibleTiles.empty())
            focusTile(m_visibleTiles, 0);
        return true;
    }

    for (Gtk::Widget *focus = get_focus(); focus; focus = focus->get_parent()) {
        if (auto *tile = dynamic_cast<EmojiTile *>(focus))
            return handleTileKey(*tile, keyval);
    }
    return false;
}

bool Window::handleTileKey(EmojiTile &tile, const guint keyval)
{
    const auto recentIt = std::find(m_recentTiles.begin(), m_recentTiles.end(), &tile);
    const bool inRecent = recentIt != m_recentTiles.end();
    const std::vector<EmojiTile *> &tiles = inRecent ? m_recentTiles : m_visibleTiles;
    const auto it = inRecent ? recentIt : std::find(tiles.begin(), tiles.end(), &tile);
    if (it == tiles.end())
        return false;
    const int index = static_cast<int>(it - tiles.begin());
    const int count = static_cast<int>(tiles.size());
    const int columns = columnCount(tiles);
    const int column = index % columns;

    switch (keyval) {
    case GDK_KEY_space:
    case GDK_KEY_Return:
    case GDK_KEY_KP_Enter:
        selectEmoji(tile.emoji());
        return true;
    case GDK_KEY_Left:
        // Left and Right stay on the current row.
        if (column > 0)
            focusTile(tiles, index - 1);
        return true;
    case GDK_KEY_Right:
        if (column < columns - 1)
            focusTile(tiles, index + 1);
        return true;
    case GDK_KEY_Down:
        if (index + columns < count) {
            focusTile(tiles, index + columns);
        } else if (inRecent && !m_visibleTiles.empty()) {
            const int last = std::min(columnCount(m_visibleTiles),
                                      static_cast<int>(m_visibleTiles.size()))
                - 1;
            focusTile(m_visibleTiles,
                      nearestTile(m_visibleTiles, 0, last, centreX(tile, *this), *this));
        }
        return true;
    case GDK_KEY_Up:
        if (index >= columns) {
            focusTile(tiles, index - columns);
        } else if (!inRecent && recentVisible()) {
            const int recentCount = static_cast<int>(m_recentTiles.size());
            const int lastRow = (recentCount - 1) / columnCount(m_recentTiles)
                * columnCount(m_recentTiles);
            focusTile(m_recentTiles, nearestTile(m_recentTiles, lastRow, recentCount - 1,
                                                 centreX(tile, *this), *this));
        } else {
            focusSearch();
            m_search.select_region(0, -1);
        }
        return true;
    default:
        // Printable keys reach the search entry through its key capture widget.
        return false;
    }
}

} // namespace emojipicker
