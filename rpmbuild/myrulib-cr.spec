#
# spec file for package myrulib-cr
#
# Copyright (c) 2009, 2010, 2011, 2012 Denis Kandrashin, Kyrill Detinov
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:           myrulib-cr
Version:        0.29.11
Release:        0
License:        GPL-3.0
Summary:        E-Book Library Manager
URL:            http://myrulib.lintest.ru
Group:          Productivity/Other
Source0:        http://www.lintest.ru/pub/myrulib_%{version}.orig.tar.bz2
# Need to build debian packages.
Source90:       myrulib_%{version}-squeeze1.debian.tar.gz
Source91:       myrulib_%{version}-squeeze1.dsc
Source92:       myrulib_%{version}-squeeze1_source.changes
BuildRequires:  gcc-c++
BuildRequires:  libicu-devel
BuildRequires:  libxml2-devel
BuildRequires:  libjpeg-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Conflicts:      myrulib

%if 0%{?suse_version}
BuildRequires:  update-desktop-files
BuildRequires:  wxWidgets-devel
%define _use_internal_dependency_generator 0
%define __find_requires %wx_requires
%endif

%if 0%{?mandriva_version}
BuildRequires:  libbzip2-devel
BuildRequires:  libwxgtku2.8-devel >= 2.8.10
%endif

%if 0%{?fedora_version}
BuildRequires:  desktop-file-utils
BuildRequires:  wxGTK-devel >= 2.8.10
%endif

%description
MyRuLib is an application for organizing your own collection of e-books.

This package includes built-in CoolReader3 engine.



Authors:
--------
    Denis Kandrashin <mail@lintest.ru>

%prep
%setup -qn myrulib-%{version}
[ ! -x configure ] && %__chmod +x configure

%build
%configure \
    --with-icu \
    --with-reader \
    --without-strip

%if 0%{?fedora_version} || 0%{?suse_version}
make LDFLAGS="-Wl,--add-needed" %{?_smp_mflags}
%else
make %{?_smp_mflags}
%endif

%install
%if 0%{?fedora_version} || 0%{?mandriva_version}
%{?buildroot:%__rm -rf "%{buildroot}"}
%endif

make DESTDIR=%{buildroot} install
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

%files -f myrulib.lang
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog LICENSE README
%{_bindir}/myrulib
%{_datadir}/applications/myrulib.desktop
%{_datadir}/icons/hicolor/*/*/myrulib.png
%{_datadir}/pixmaps/myrulib.png

%changelog
