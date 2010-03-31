#
# spec file for package myrulib (Version 0.20)
#
# Copyright (c) 2009-2010 Denis Kandrashin, Kyrill Detinov
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:		myrulib
Version:	0.21
Release:	0
License:	GPL
Summary:	E-Book Library Manager
URL:		http://myrulib.lintest.ru
Group:		Productivity/Other
Source0:	%{name}-%{version}.tar.bz2
BuildRequires:	gcc-c++
BuildRoot:	%{_tmppath}/%{name}-%{version}-build

%if 0%{?suse_version}
BuildRequires:	wxGTK-devel >= 2.8.10
BuildRequires:	sqlite3-devel
BuildRequires:	libexpat-devel
BuildRequires:	update-desktop-files
%endif

%if 0%{?mandriva_version}
BuildRequires:	libwxgtku2.8-devel >= 2.8.10
BuildRequires:	libexpat-devel
%if %{_arch} == x86_64
BuildRequires:	lib64sqlite3-devel
%else
BuildRequires:	libsqlite3-devel
%endif
%endif

%if 0%{?fedora_version}
BuildRequires:	wxGTK-devel >= 2.8.10
BuildRequires:	sqlite-devel
BuildRequires:	expat-devel
BuildRequires:	desktop-file-utils
%endif

%description
MyRuLib is an application for organizing your own collection of e-books.

Authors:
--------
    Denis Kandrashin <mail@lintest.ru>

%prep
%setup -q

%build
export CXXFLAGS="%{optflags}"
export CFLAGS="%{optflags}"
make %{?_smp_mflags}

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
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png  
%{_datadir}/pixmaps/%{name}.png  

%changelog
