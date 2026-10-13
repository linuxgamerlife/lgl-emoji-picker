# LGL Emoji Picker v1.5

LGL Emoji Picker has moved from Qt 6 to GTK 4 and libadwaita (still C++, via
gtkmm).

Highlights:

- Follows your desktop's dark mode, accent colour and GTK theme, and re-themes
  live when `~/.config/gtk-4.0/gtk.css` changes (Noctalia and similar).
- New About dialog in the header bar menu.
- Launching the picker again brings the open window to the front.
- Search, recent history, tooltips, keyboard navigation and clipboard behaviour
  are unchanged, and your existing recent history is kept.

Install on Fedora:

```sh
sudo dnf copr enable linuxgamerlife/lgl-emoji-picker
sudo dnf install lgl-emoji-picker
```
