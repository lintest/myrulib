#!/bin/bash
##################################################################
#                                                                #
#  This file provides a cross-compilation for Mac OS             #
#                                                                #
##################################################################

WXW_VERSION="3.0.0"   # Version wxWidgets
FT2_VERSION="2.5.3"   # Version FreeType2

##################################################################
# Detect MyRuLib version
##################################################################

MRL_VERSION=`cat configure | grep PACKAGE_VERSION= | sed "s/.*=//" | sed "s/'//g"`

WXW_FILENAME="wxWidgets-$WXW_VERSION"
FT2_FILENAME="freetype-$FT2_VERSION"
MRL_FILENAME="myrulib-$MRL_VERSION"
MAC_SDK_PATH="/Developer/SDKs/MacOSX10.6.sdk"
MAC_SDK_PATH="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"
arch_flags="-arch i386 -isysroot $MAC_SDK_PATH -mmacosx-version-min=10.9"

SOURCE_DIR=`pwd`
		
cd ~

mkdir $MRL_FILENAME
cd $MRL_FILENAME

BUILD_DIR=`pwd`

##################################################################
# wxWidgets
##################################################################

EXISTS_WXW=`$BUILD_DIR/bin/wx-config --version`

if test "x$EXISTS_WXW" != "x$WXW_VERSION" ; then 

  cd $BUILD_DIR
  rm -rf $WXW_FILENAME
  rm $WXW_FILENAME.tar.bz2
  curl -LO http://sourceforge.net/projects/wxwindows/files/$WXW_VERSION/$WXW_FILENAME.tar.bz2
  tar -xvjf $WXW_FILENAME.tar.bz2
  ln -s $WXW_FILENAME wxWidgets
  cd $WXW_FILENAME
  mkdir build_mac
  cd build_mac
  arch_flags="-arch i386"
  ../configure \
    CFLAGS="$arch_flags" \
    CXXFLAGS="$arch_flags" \
    OBJCFLAGS="$arch_flags" \
    OBJCXXFLAGS="$arch_flags" \
    LDFLAGS="$arch_flags" \
    --with-osx_cocoa \
    --with-macosx-sdk="$MAC_SDK_PATH" \
    --with-macosx-version-min=10.9 \
    --prefix=$BUILD_DIR \
    --enable-unicode \
    --disable-shared \
    --without-expat \
    --without-regex \
    --without-libtiff \
    --disable-richtext \
    --with-libjpeg=builtin \
    --with-libpng=builtin \
    --with-zlib=builtin \

  make
  make install

fi

##################################################################
# FreeType2
##################################################################


EXISTS_FT2=`$BUILD_DIR/bin/freetype-config --ftversion`

if test "x$EXISTS_FT2" != "x$FT2_VERSION" ; then 

  cd $BUILD_DIR
  rm -rf $FT2_FILENAME
  rm $FT2_FILENAME.tar.bz2
  curl -LO http://sourceforge.net/projects/freetype/files/freetype2/$FT2_VERSION/$FT2_FILENAME.tar.bz2
  tar -xvjf $FT2_FILENAME.tar.bz2
  cd $FT2_FILENAME
  mkdir build_mac
  cd build_mac
  ../configure \
    CFLAGS="$arch_flags" \
    CXXFLAGS="$arch_flags" \
    OBJCFLAGS="$arch_flags" \
    OBJCXXFLAGS="$arch_flags" \
    LDFLAGS="$arch_flags" \
    --prefix=$BUILD_DIR \
    --enable-universal-binary \
    --disable-shared
  make
  make install

fi

##################################################################
# MyRuLib
##################################################################

cd $SOURCE_DIR
rm -rf build_mac
mkdir build_mac
cd build_mac
../configure \
  CFLAGS="$arch_flags" \
  CXXFLAGS="$arch_flags" \
  OBJCFLAGS="$arch_flags" \
  OBJCXXFLAGS="$arch_flags" \
  LDFLAGS="$arch_flags" \
    --with-wx-prefix=$BUILD_DIR \
    --with-wxsqlite \
    --with-expat \
    --with-bzip2
make

cd $SOURCE_DIR
rm -rf build_cr3
mkdir build_cr3
cd build_cr3
../configure \
  CFLAGS="$arch_flags" \
  CXXFLAGS="$arch_flags -I$BUILD_DIR/$WXW_FILENAME/src/\
  -I$BUILD_DIR/$WXW_FILENAME/src/jpeg\
  -I$BUILD_DIR/$WXW_FILENAME/src/png\
  -I$BUILD_DIR/$WXW_FILENAME/src/zlib\
  -DCR_EMULATE_GETTEXT=1" \
  OBJCFLAGS="$arch_flags" \
  OBJCXXFLAGS="$arch_flags" \
  LDFLAGS="$arch_flags" \
    --with-wx-prefix=$BUILD_DIR \
    --with-ft-prefix=$BUILD_DIR \
    --disable-freetypetest \
    --with-wxsqlite \
    --with-expat \
    --with-bzip2 \
    --with-reader
make
