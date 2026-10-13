# Changelog

## v1.5 - 2026-10-13

- Migrated the app from Qt 6 to GTK 4 and libadwaita, still written in C++ (gtkmm). The GTK app lives in `gtk/` and is built from the repository root; the Qt 6 sources in `src/` are kept for reference and are no longer built.
- The UI follows the desktop's dark mode, accent colour and `~/.config/gtk-4.0/gtk.css` (Noctalia and similar generators), and re-themes live when that file changes.
- Added an About dialog, opened from the header bar menu.
- Launching the picker again brings the open window to the front instead of starting a second copy.
- Renamed the desktop and AppStream files to `io.github.linuxgamerlife.lgl-emoji-picker`.
- Recent emoji history from earlier versions is kept (`~/.cache/lgl-emoji-picker/recent.txt`).
- Added unit tests for search, recent history, clipboard helpers and theme discovery, run in CI and during the RPM build.
- RPM source archives are now generated with `make srpm` (also used by COPR) instead of downloading a Git tag.

## v1.0.4 - 2026-05-24

- Hovering over an emoji now shows a tooltip with the emoji name.

## v1.0.3 - 2026-05-16

- Fixed [GitHub issue #1](https://github.com/linuxgamerlife/lgl-emoji-picker/issues/1): arrow keys now navigate through the emoji grid.
- Added a visible selected/focused state while navigating emojis with the keyboard.
- Added Space and Enter activation for the focused emoji.
- Fixed [GitHub issue #2](https://github.com/linuxgamerlife/lgl-emoji-picker/issues/2): the emoji grid fills the available window space when resized.
- Reduced the short pause when clearing the search field by reusing emoji grid buttons instead of recreating the full catalog.

## v1.0.2 - 2026-05-15

- COPR rebuild release after the initial package publication.

## v1.0.1 - 2026-05-15

- Initial release.
