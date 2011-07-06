#
# spec file for package myrulib-cr (Version 0.28)
#
# Copyright (c) 2009-2011 Denis Kandrashin, Kyrill Detinov
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:           myrulib-cr
Version:        0.28.6
Release:        0
License:        GPLv3
Summary:        E-Book Library Manager
URL:            http://myrulib.lintest.ru
Group:          Productivity/Other
Source0:        http://www.lintest.ru/pub/%{name}-%{version}.tar.bz2
# PATCH-FIX-UPSTREAM myrulib-0.28-cregine_png14.patch lazy.kent@opensuse.org -- fix build against libpng 1.4
Patch0:         myrulib-0.28-crengine_png14.patch
# PATCH-FIX-UPSTREAM myrulib-0.28-crengine_lvfntman.patch mail@lintest.ru -- fix build crengine lvfntman
Patch1:         myrulib-0.28-crengine_lvfntman.patch
BuildRequires:  gcc-c++
BuildRequires:  libfaxpp-devel
BuildRequires:  libjpeg-devel
Conflicts:      myrulib
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%if 0%{?suse_version}
BuildRequires:  update-desktop-files
%if 0%{?suse_version} >= 1140
BuildRequires:  sqlite3-devel
BuildRequires:  wxWidgets-devel
%else
BuildRequires:  wxGTK-devel >= 2.8.10
%endif
%endif

%if 0%{?mandriva_version}
BuildRequires:  libwxgtku2.8-devel >= 2.8.10
%endif

%if 0%{?fedora_version}
BuildRequires:  desktop-file-utils
BuildRequires:  wxGTK-devel >= 2.8.10
%if 0%{?fedora_version} >= 15
BuildRequires:  libsqlite3x-devel
%endif
%endif

%description
MyRuLib is an application for organizing your own collection of e-books.

This package includes built-in CoolReader3 engine.



Authors:
--------
    Denis Kandrashin <mail@lintest.ru>

%prep
%setup -q
%patch0
%patch1
[ ! -x configure ] && %__chmod +x configure

%build
%configure \
            --with-faxpp=yes \
            --with-reader \
            --without-strip

%if 0%{?fedora_version} >= 13
%__make LDFLAGS="-Wl,--add-needed" %{?_smp_mflags}
%else
%__make %{?_smp_mflags}
%endif

%install
%if 0%{?fedora_version} || 0%{?mandriva_version}
%{?buildroot:%__rm -rf "%{buildroot}"}
%endif

%__make DESTDIR=%{buildroot} install
%find_lang myrulib

%if 0%{?suse_version}
%suse_update_desktop_file myrulib
%endif

%if 0%{?fedora_version}
desktop-file-validate %{buildroot}%{_datadir}/applications/myrulib.desktop
%endif

%if 0%{?suse_version} >= 1140
%post
%desktop_database_post
%icon_theme_cache_post

%postun
%desktop_database_postun
%icon_theme_cache_postun
%endif

%if 0%{?mandriva_version}
%post
%update_menus

%postun
%clean_menus
%endif

%clean
%{?buildroot:%__rm -rf "%{buildroot}"}

%files -f myrulib.lang
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog LICENSE README
%{_bindir}/myrulib
%{_datadir}/applications/myrulib.desktop
%{_datadir}/icons/hicolor/*/*/myrulib.png
%{_datadir}/pixmaps/myrulib.png

%changelog
