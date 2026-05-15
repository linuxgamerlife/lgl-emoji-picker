# LGL Emoji Picker v1

A small Qt 6 emoji picker for desktop use.

This app is based on the original Python emoji picker script by TheBlackDon from
[`mango-waybar`](https://codeberg.org/theblackdon/mango-waybar). Version 1 ports
the picker to Qt 6/C++, keeps the emoji catalog and recent-history workflow, and
uses native Qt styling.

## Current Status

- Native Qt 6 application.
- Search by emoji name or emoji character.
- Click an emoji, or press Enter after searching, to copy the first result.
- The app stays open after selection for repeated use.
- Recent selections are shown at the top.
- Recent emoji history is stored at `~/.cache/lgl-emoji-picker/recent.txt`.
- Recent history loading is capped and saved atomically to avoid corrupt cache files.
- Clipboard support uses Qt clipboard data first, with Wayland/X11 command-line
  fallbacks where available.
- `wl-clipboard` is optional; it can improve Wayland clipboard behavior, but the
  app works through Qt while it remains open.
- The UI follows the active Qt platform theme and palette.
- A small in-app credit shows `Inspired by TheBlackDon` with the bundled
  `references/Black-Don.png` image.

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Run

```sh
./build/lgl-emoji-picker
```

## Install

```sh
cmake --install build
```

The installed desktop file runs `lgl-emoji-picker`, so it can be bound to a
keyboard shortcut such as `Super+Alt+E`.

## Packaging

RPM/COPR packaging lives in `packaging/lgl-emoji-picker.spec`. The package
installs:

- `/usr/bin/lgl-emoji-picker`
- `/usr/share/applications/lgl-emoji-picker.desktop`
- `/usr/share/metainfo/lgl-emoji-picker.metainfo.xml`
- `/usr/share/icons/hicolor/48x48/apps/lgl-emoji-picker.png`
- `/usr/share/icons/hicolor/64x64/apps/lgl-emoji-picker.png`
- `/usr/share/icons/hicolor/128x128/apps/lgl-emoji-picker.png`
- `/usr/share/icons/hicolor/256x256/apps/lgl-emoji-picker.png`

Build dependencies are declared in the spec. `wl-clipboard` is a weak dependency
(`Recommends`) rather than a hard requirement.
