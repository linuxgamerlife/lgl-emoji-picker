Name:           lgl-emoji-picker
Version:        1.0.2
Release:        1%{?dist}
Summary:        Qt emoji picker with search and recent history

License:        MIT
URL:            https://github.com/linuxgamerlife/lgl-emoji-picker
Source0:        %{url}/archive/refs/tags/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  desktop-file-utils
BuildRequires:  appstream

Requires:       google-noto-color-emoji-fonts
Recommends:     wl-clipboard

%description
LGL Emoji Picker is a small Qt 6 emoji picker inspired by TheBlackDon's Python
script. It supports emoji search, recent history, native Qt styling, and
clipboard integration for Wayland and X11 sessions.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake
%cmake_build

%install
%cmake_install

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop
appstreamcli validate --no-net %{buildroot}%{_datadir}/metainfo/%{name}.metainfo.xml

%files
%license LICENSE
%doc README.md
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/metainfo/%{name}.metainfo.xml
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png
%{_datadir}/icons/hicolor/64x64/apps/%{name}.png
%{_datadir}/icons/hicolor/128x128/apps/%{name}.png
%{_datadir}/icons/hicolor/256x256/apps/%{name}.png

%changelog
* Fri May 15 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.2-1
- Add google-noto-color-emoji-fonts runtime dependency for reliable color emoji rendering

* Fri May 15 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.1-1
- v1.0.1 release with bundled hicolor app icons and updated desktop metadata

* Fri May 15 2026 linuxgamerlife <linuxgamerlife@users.noreply.github.com> - 1.0.0-1
- Initial v1 package for COPR SCM builds
