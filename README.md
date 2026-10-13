# LGL Emoji Picker v1.5

A small GTK 4 emoji picker for desktop use.

This app is based on the original Python emoji picker script by TheBlackDon from
[`mango-waybar`](https://codeberg.org/theblackdon/mango-waybar). Version 1.5 moves the app from Qt 6 to GTK 4 and libadwaita (still C++, via gtkmm). It keeps
the emoji catalog and recent-history workflow, and follows the desktop theme.

## Current Status

- Native GTK 4 / libadwaita application, written in C++ with gtkmm.
- Search by emoji name or emoji character.
- Click an emoji, or press Enter after searching, to copy the first result.
- Hover over an emoji to see its name in a tooltip.
- Navigate emojis with arrow keys, with a visible focused selection.
- Press Space or Enter to copy the focused emoji.
- The app stays open after selection for repeated use.
- Recent selections are shown at the top.
- Recent emoji history is stored at `~/.cache/lgl-emoji-picker/recent.txt`.
- Recent history loading is capped and saved atomically to avoid corrupt cache files.
- Clipboard support uses the GTK clipboard first, with Wayland/X11 command-line
  fallbacks where available.
- `google-noto-color-emoji-fonts` is required on Fedora for reliable color emoji rendering.
- `wl-clipboard` is optional; it can improve Wayland clipboard behavior, but the
  app works through GTK while it remains open.
- The UI follows the desktop's dark mode, accent colour and
  `~/.config/gtk-4.0/gtk.css` (Noctalia and similar generators), and re-themes
  live when that file changes.
- Launching the picker again brings the open window to the front.
- The header bar menu has an About dialog and Quit (`Ctrl+Q`).
- A small in-app credit shows `Inspired by TheBlackDon` with the bundled
  `references/Black-Don.png` image.

## Install

The recommended install path on Fedora is COPR:

```sh
sudo dnf copr enable linuxgamerlife/lgl-emoji-picker
sudo dnf install lgl-emoji-picker
```

Then run it from your app launcher or with:

```sh
lgl-emoji-picker
```

## Build From Source

The packaged application is the GTK build in `gtk/`, built from the repository
root. On Fedora, install the build dependencies first:

```sh
sudo dnf install cmake ninja-build gcc-c++ gtkmm4.0-devel libadwaita-devel glib2-devel
```

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

Run from the build directory:

```sh
./build/lgl-emoji-picker
```

Install from source:

```sh
sudo cmake --install build
```

The Qt 6 sources in `src/` are the legacy app, kept for reference only and not
built.

The installed desktop file runs `lgl-emoji-picker`, so it can be bound to a
keyboard shortcut such as `Super+Alt+E`.
