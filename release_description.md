# LGL Emoji Picker v1.0.2

Small maintenance release for the Qt 6 emoji picker. This release keeps the app open after selections, supports search and recent emoji history, copies through Qt clipboard with Wayland/X11 fallbacks, includes bundled app icons for desktop launchers and docks, and declares `google-noto-color-emoji-fonts` as a Fedora runtime dependency for reliable color emoji rendering.

Install on Fedora:

```sh
sudo dnf copr enable linuxgamerlife/lgl-emoji-picker
sudo dnf install lgl-emoji-picker
```
