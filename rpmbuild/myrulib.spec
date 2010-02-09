%define name     myrulib
%define version  0.19
%define release  fedora12

Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Summary: E-Book Library Manager
URL: http://myrulib.lintest.ru
Group: Productivity/Other
Source0: %{name}_%{version}.tar.gz
Requires: wxGTK >= 2.8.10
BuildRequires:  wxGTK-devel >= 2.8.10 sqlite3-devel libexpat-devel gcc-c++
BuildRequires:  update-desktop-files
BuildRoot: %{_tmppath}

%description
MyRuLib is an application for organizing your own collection of e-books.

Authors:
--------
    Denis Kandrashin <mail@lintest.ru>

%prep
%setup -q -a 0

%build
make

%install
%{makeinstall}

%files
%defattr(-,root,root)
%{_bindir}/myrulib
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png

%clean
make clean

%post

%changelog
* Tue Feb 09 2010 Denis Kandrashin
- Update release
* Wed Jan 20 2010 Denis Kandrashin
- Update release
* Sat Nov 07 2009 Denis Kandrashin
- Initial build
