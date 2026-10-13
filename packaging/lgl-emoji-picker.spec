Name:           lgl-emoji-picker
Version:        1.5.2
Release:        1%{?dist}
Summary:        GTK emoji picker with search and recent history

License:        MIT
URL:            https://github.com/linuxgamerlife/lgl-emoji-picker
# Generated from the checked-out commit by `make source` or `make srpm`.
# COPR SCM builds do not require a Git tag.
Source0:        %{name}-%{version}.tar.gz

# The root CMakeLists.txt builds the GTK 4 application in gtk/; the Qt sources
# in src/ are not built.
%global app_id com.linuxgamerlife.lgl-emoji-picker

BuildRequires:  cmake >= 3.20
BuildRequires:  cmake-rpm-macros
BuildRequires:  gcc-c++
BuildRequires:  ninja-build
BuildRequires:  pkgconfig(gtkmm-4.0) >= 4.20
BuildRequires:  pkgconfig(libadwaita-1) >= 1.6
BuildRequires:  glib2-devel
BuildRequires:  desktop-file-utils
BuildRequires:  appstream

Requires:       google-noto-color-emoji-fonts
Recommends:     wl-clipboard

%description
LGL Emoji Picker is a small GTK 4 emoji picker inspired by TheBlackDon's Python
script. It supports emoji search, recent history, libadwaita styling that
follows the desktop theme, and clipboard integration for Wayland and X11
sessions.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
%cmake_build

%install
%cmake_install

%check
%ctest
desktop-file-validate %{buildroot}%{_datadir}/applications/%{app_id}.desktop
appstreamcli validate --no-net %{buildroot}%{_datadir}/metainfo/%{app_id}.metainfo.xml

%files
%license LICENSE
%doc README.md CHANGELOG.md
%{_bindir}/%{name}
%{_datadir}/applications/%{app_id}.desktop
%{_datadir}/metainfo/%{app_id}.metainfo.xml
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png
%{_datadir}/icons/hicolor/64x64/apps/%{name}.png
%{_datadir}/icons/hicolor/128x128/apps/%{name}.png
%{_datadir}/icons/hicolor/256x256/apps/%{name}.png

%changelog
* Tue Oct 13 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.5.2-1
- Change the application ID to com.linuxgamerlife.lgl-emoji-picker
- Show the app icon and wordmark in the header bar
- Add a system title bar mode, toggled with Ctrl+Shift+M and remembered
- Open and close the main menu by tapping Alt
- Apply the desktop monospace font to the wordmark

* Tue Oct 13 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.5-1
- Migrate from Qt 6 to GTK 4 (gtkmm) and libadwaita
- Follow the desktop theme live, including ~/.config/gtk-4.0/gtk.css changes
- Add an About dialog
- Present the running window when launched again
- Rename desktop and AppStream files to io.github.linuxgamerlife.lgl-emoji-picker
- Generate the source archive with make srpm instead of a Git tag

* Sun May 24 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.4-1
- Show emoji name tooltip on hover

* Sat May 16 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.3-1
- Fix arrow-key emoji navigation and add visible keyboard focus selection
- Add Space and Enter activation for the focused emoji
- Cache emoji grid buttons to avoid a short freeze when clearing search

* Fri May 15 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.2-1
- COPR rebuild release after the initial package publication

* Fri May 15 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.1-1
- Initial release

* Fri May 15 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.0-1
- Initial v1 package for COPR SCM builds
