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
- Clipboard support uses Qt clipboard data plus Wayland/X11 command-line
  fallbacks where available.
- The UI follows the active Qt platform theme and palette.

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
