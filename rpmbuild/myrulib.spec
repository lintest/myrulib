#
# spec file for package myrulib
#
# Copyright (c) 2009-2013 Denis Kandrashin, Kyrill Detinov
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:           myrulib
Version:        0.29.16
Release:        0
License:        GPL-3.0
Summary:        E-Book Library Manager
URL:            http://myrulib.lintest.ru
Group:          Productivity/Other
Source0:        http://www.lintest.ru/pub/%{name}_%{version}.orig.tar.bz2
Source90:       %{name}_%{version}-squeeze1.debian.tar.gz
Source91:       %{name}_%{version}-squeeze1.dsc
Source92:       %{name}_%{version}-squeeze1_source.changes
BuildRequires:  gcc-c++
BuildRequires:  libicu-devel
BuildRequires:  libxml2-devel
Conflicts:      myrulib-cr
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%if 0%{?suse_version}
BuildRequires:  update-desktop-files
BuildRequires:  wxWidgets-devel
%define _use_internal_dependency_generator 0
%define __find_requires %wx_requires
%endif

%if 0%{?mandriva_version}
BuildRequires:  libwxgtku2.8-devel >= 2.8.10
BuildRequires:  libbzip2-devel
%endif

%if 0%{?fedora_version}
BuildRequires:  wxGTK-devel >= 2.8.10
BuildRequires:  desktop-file-utils
%endif

%description
MyRuLib is an application for organizing your own collection of e-books.



Authors:
--------
    Denis Kandrashin <mail@lintest.ru>

%prep
%setup -q
[ ! -x configure ] && %{__chmod} +x configure

%build
%configure \
    --with-icu \
    --without-strip

%if 0%{?fedora_version}
make LDFLAGS="-Wl,--add-needed" %{?_smp_mflags}
%else
make %{?_smp_mflags}
%endif

%install
%if 0%{?fedora_version} || 0%{?mandriva_version}
rm -rf %{buildroot}
%endif

make DESTDIR=%{buildroot} install
%find_lang %{name}

%if 0%{?suse_version}
%suse_update_desktop_file %{name}
%endif

%if 0%{?fedora_version}
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop
%endif

%if 0%{?suse_version}
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
rm -rf %{buildroot}

%files -f %{name}.lang
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/*/%{name}.png
%{_datadir}/pixmaps/%{name}.png

%changelog
