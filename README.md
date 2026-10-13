<div align="center">

# LGL Emoji Picker

**A fast, keyboard-driven emoji picker that copies to your clipboard and stays open.**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Release](https://img.shields.io/badge/release-1.5-purple)](CHANGELOG.md)
[![Fedora](https://img.shields.io/badge/Fedora-44-blue?logo=fedora&logoColor=white)](https://fedoraproject.org)
[![RPM](https://img.shields.io/badge/RPM-available-brightgreen?logo=redhat&logoColor=white)](https://copr.fedorainfracloud.org/coprs/linuxgamerlife/lgl-emoji-picker/)
[![GTK](https://img.shields.io/badge/GTK-4-green?logo=gtk&logoColor=white)](https://www.gtk.org)
[![Support on Ko-fi](https://img.shields.io/badge/Support_on-Ko--fi-FF5E5B?logo=ko-fi&logoColor=white)](https://ko-fi.com/linuxgamerlife)

</div>

---

## Overview

LGL Emoji Picker is a small emoji picker for Linux. Bind it to a keyboard
shortcut, search for an emoji by name, and copy it to the clipboard without
leaving the keyboard.

- Search by emoji name or by pasting the emoji itself
- Copy with a click, Enter, or Space, and keep the picker open for more
- See your recent emojis at the top
- Navigate the whole grid with the arrow keys
- Match your desktop: the picker follows your GTK theme and colours live
- Works on Wayland and X11

It is based on the original Python emoji picker script by TheBlackDon from
[`mango-waybar`](https://codeberg.org/theblackdon/mango-waybar).

---

## How It Works

1. Launch the picker from your app launcher or a keyboard shortcut such as
   `Super+Alt+E`. Launching it again brings the open window to the front.
2. Start typing to search. Press **Enter** to copy the first result.
3. Or press **Down** to move into the grid, use the arrow keys to choose an
   emoji, and press **Space** or **Enter** to copy it. Clicking an emoji copies
   it too.
4. The picker stays open, so you can copy several emojis in a row. Press
   **Escape** to close it.

---

## Features

| Feature | What it does |
|---|---|
| **Follows your theme** | Uses your GTK palette, light or dark mode, and accent colour, and updates live when `~/.config/gtk-4.0/gtk.css` changes (Noctalia and similar) |
| **Search** | Filter by emoji name, case-insensitively, or by the emoji character |
| **Recent emojis** | Show your most recent picks, centred above the full grid |
| **Keyboard navigation** | Move between search, recent emojis, and the grid with the arrow keys |
| **Quick copy** | Copy with a click, Enter, or Space; Enter in the search field copies the first result |
| **Tooltips** | Hover over an emoji to see its name |
| **Stays open** | Keep the picker open for repeated selections |
| **Single window** | Launching the picker again brings the open window to the front |
| **Clipboard support** | Copy through GTK, with `wl-copy`, `xclip`, or `xsel` as fallbacks |
| **About** | Show the version, project and author links, licence, credits, and Ko-fi support link |

---

## Install

The current release is **1.5**.

### Recommended — Fedora COPR

Enable the [LGL Emoji Picker COPR](https://copr.fedorainfracloud.org/coprs/linuxgamerlife/lgl-emoji-picker/)
and install the application:

```bash
sudo dnf copr enable linuxgamerlife/lgl-emoji-picker
sudo dnf install lgl-emoji-picker
```

Future releases will then be installed through normal Fedora system updates.

The package installs `google-noto-color-emoji-fonts` for reliable colour emoji
rendering. `wl-clipboard` is recommended but optional; the picker copies
through GTK while it is open.

Run it from your app launcher or with:

```bash
lgl-emoji-picker
```

The project can also be built directly from source.

---

## What's New in 1.5

- Migrated from Qt 6 to GTK 4 and libadwaita, still written in C++
- Follows your desktop GTK theme live: colour palette (including Noctalia),
  light or dark mode, and accent colour
- Recent emojis and the emoji grid each have their own card
- New About dialog in the header bar menu
- Launching the picker again brings the open window to the front
- Your existing recent emoji history is kept

See [CHANGELOG.md](CHANGELOG.md) for the complete release history.

---

## Build from Source

LGL Emoji Picker is a GTK 4 and libadwaita application in the `gtk/` folder,
built from the repository root. It requires gtkmm 4.20 and libadwaita 1.6 or
newer.

Install the dependencies on Fedora 44:

```bash
sudo dnf install cmake gcc-c++ gtkmm4.0-devel libadwaita-devel
```

Clone, build, and test:

```bash
git clone https://github.com/linuxgamerlife/lgl-emoji-picker.git
cd lgl-emoji-picker
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Run it directly from the build directory:

```bash
./build/lgl-emoji-picker
```

Install it system-wide:

```bash
sudo cmake --install build
```

Ubuntu 24.04 and Linux Mint 22 ship older GTK libraries and cannot build 1.5
from source.

The Qt 6 sources in `src/` are the previous version of the app, kept for
reference only and not built.

---

## Privacy and Data

Recent emojis are stored locally in `~/.cache/lgl-emoji-picker/recent.txt`.
History loading is capped, and the file is saved atomically so it cannot be
left half-written. Nothing is uploaded.

Clipboard fallbacks run `wl-copy`, `xclip`, or `xsel` from fixed paths without
a shell, and the emoji is passed on standard input.

---

## Credits

Inspired by TheBlackDon's original Python emoji picker from
[`mango-waybar`](https://codeberg.org/theblackdon/mango-waybar).

---

## License

MIT — see [LICENSE](LICENSE) for details.

---

<div align="center">
Made for <a href="https://fedoraproject.org">Fedora</a> · by <a href="https://www.youtube.com/@linuxgamerlife">LinuxGamerLife</a>
</div>
