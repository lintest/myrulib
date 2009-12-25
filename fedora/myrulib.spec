%define name     myrulib
%define version  0.17
%define release  fedora11

Name: %{name}
Version: %{version}
Release: %{release}
Summary: Create your own collection of e-books
Group: Applications/Office
License: GPL
Source0: %{name}_%{version}.tar.gz
BuildRoot: %{_tmppath}
Requires: wxGTK >= 2.8.10

%description
MyRuLib — свободная (бесплатная и открытая) программа
для организации домашней библиотеки (коллекции)
электронных книг формата fb2.
http://myrulib.lintest.ru

%prep
%setup -q -a 0

%build
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root)
%{_bindir}/myrulib
/usr/share/applications/myrulib.desktop
/usr/share/myrulib/home-32x32.png

%clean
make clean

%post

%changelog
* Sat Nov 07 2009 Denis Kandrashin
- Initial build
