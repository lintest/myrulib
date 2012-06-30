#!/bin/sh

VERSION=`cat configure | grep PACKAGE_VERSION= | sed "s/.*=//" | sed "s/'//g"`  
echo Version: $VERSION;

rm -rf tarball
mkdir tarball
cd tarball

mkdir myrulib-$VERSION
cd myrulib-$VERSION

cp -f ../../AUTHORS          .
cp -f ../../INSTALL          .
cp -f ../../LICENSE          .
cp -f ../../README           .
cp -f ../../ChangeLog        .
cp -f ../../aclocal.m4       .
cp -f ../../autoconf_inc.m4  .
cp -f ../../config.guess     .
cp -f ../../config.sub       .
cp -f ../../configure        .
cp -f ../../configure.in     .
cp -f ../../install-sh       .
cp -f ../../Makefile.in      .
cp -f ../../myrulib.bkl      .
cp -f ../../windows.sh       .
cp -rf ../../3rdparty        .

mkdir sources

cp -rf ../../sources/MyRuLib     sources
cp -rf ../../sources/WxSQLite3   sources
cp -rf ../../sources/ZipScaner   sources
cp -f  ../../sources/config.h.in sources

cd ..

tar -cvjf myrulib-$VERSION.tar.bz2 myrulib-$VERSION
cp myrulib-$VERSION.tar.bz2 myrulib_$VERSION.orig.tar.bz2

cp -rf myrulib-$VERSION myrulib-cr-$VERSION
tar -cvjf myrulib-cr_$VERSION.orig.tar.bz2 myrulib-cr-$VERSION
