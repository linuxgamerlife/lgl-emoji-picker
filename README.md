# LGL Emoji Picker v1.0.3

A small Qt 6 emoji picker for desktop use.

This app is based on the original Python emoji picker script by TheBlackDon from
[`mango-waybar`](https://codeberg.org/theblackdon/mango-waybar). Version 1.0.3 is a Qt 6/C++ app that keeps the emoji catalog and recent-history workflow, and
uses native Qt styling.

## Current Status

- Native Qt 6 application.
- Search by emoji name or emoji character.
- Click an emoji, or press Enter after searching, to copy the first result.
- Navigate emojis with arrow keys, with a visible focused selection.
- Press Space or Enter to copy the focused emoji.
- The app stays open after selection for repeated use.
- Recent selections are shown at the top.
- Recent emoji history is stored at `~/.cache/lgl-emoji-picker/recent.txt`.
- Recent history loading is capped and saved atomically to avoid corrupt cache files.
- Clipboard support uses Qt clipboard data first, with Wayland/X11 command-line
  fallbacks where available.
- `google-noto-color-emoji-fonts` is required on Fedora for reliable color emoji rendering.
- `wl-clipboard` is optional; it can improve Wayland clipboard behavior, but the
  app works through Qt while it remains open.
- The UI follows the active Qt platform theme and palette.
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

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run from the build directory:

```sh
./build/lgl-emoji-picker
```

Install from source:

```sh
sudo cmake --install build
```

The installed desktop file runs `lgl-emoji-picker`, so it can be bound to a
keyboard shortcut such as `Super+Alt+E`.
