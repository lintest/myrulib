%define name     myrulib
%define version  0.19
%define relrase  1

Name: %{name}
Version: %{version}
Release: 1
License: GPL
Summary: E-Book Library Manager
URL: http://myrulib.lintest.ru
Group: Productivity/Other
Source0: %{name}_%{version}.tar.gz
Requires: wxGTK >= 2.8.10
BuildRequires: wxGTK-devel >= 2.8.10 
BuildRequires: gcc-c++
BuildRoot: %{_tmppath}/%{name}-%{version}-build  

%if 0%{?suse_version}
BuildRequires: sqlite3-devel 
BuildRequires: libexpat-devel 
BuildRequires: update-desktop-files
%endif
%if 0%{?mdkversion_version}
BuildRequires: sqlite-devel
BuildRequires: libexpat-devel 
%endif
%if 0%{?fedora_version}
BuildRequires: sqlite-devel 
BuildRequires: expat-devel
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
make

%install
make DESTDIR=%buildroot install 
%if 0%{?suse_version}
%suse_update_desktop_file %name
%endif

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png

%clean
rm -rf %{buildroot}

%changelog
* Wed Jan 20 2010 Denis Kandrashin
- Update release
* Sat Nov 07 2009 Denis Kandrashin
- Initial build
