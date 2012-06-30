#!/bin/sh
##################################################################
#                                                                #
#  This file provides a cross-compilation from Linux to Windows  #
#                                                                #
##################################################################

WXW_VERSION="2.8.12"   # Version wxWidgets
FT2_VERSION="2.4.10"   # Version FreeType2

##################################################################
# Detect MyRuLib version
##################################################################

MRL_VERSION=`cat configure | grep PACKAGE_VERSION= | sed "s/.*=//" | sed "s/'//g"`

WXW_FILENAME="wxWidgets-$WXW_VERSION"
FT2_FILENAME="freetype-$FT2_VERSION"
MRL_FILENAME="myrulib-$MRL_VERSION"

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
  rm $WXW_FILENAME.tar.bz2
  rm -rf $WXW_FILENAME
  wget http://sourceforge.net/projects/wxwindows/files/$WXW_VERSION/$WXW_FILENAME.tar.bz2
  tar -xvjf $WXW_FILENAME.tar.bz2
  ln -s $WXW_FILENAME wxWidgets
  cd $WXW_FILENAME
  mkdir build_msw
  cd build_msw
  ../configure \
	  --prefix=$BUILD_DIR \
	  --host=i686-w64-mingw32 \
	  --enable-unicode \
	  --without-expat \
	  --without-regex \
	  --without-libtiff \
	  --with-libjpeg=builtin \
	  --with-libpng=builtin \
	  --with-zlib=builtin \
	  --disable-shared
  make
  make install

fi

##################################################################
# FreeType2
##################################################################

EXISTS_FT2=`$BUILD_DIR/bin/freetype-config --ftversion`

if test "x$EXISTS_FT2" != "x$FT2_VERSION" ; then 

  cd $BUILD_DIR
  rm $FT2_FILENAME.tar.bz2
  rm -rf $FT2_FILENAME
  wget http://sourceforge.net/projects/freetype/files/freetype2/$FT2_VERSION/$FT2_FILENAME.tar.bz2
  tar -xvjf $FT2_FILENAME.tar.bz2
  cd $FT2_FILENAME
  mkdir build_msw
  cd build_msw
  ../configure \
	  --prefix=$BUILD_DIR \
	  --host=i686-w64-mingw32 \
	  --disable-shared
  make
  make install

fi

##################################################################
# MyRuLib
##################################################################

cd $SOURCE_DIR
rm -rf build_msw
mkdir build_msw
cd build_msw
LDFLAGS="-static-libgcc -static-libstdc++" ../configure \
    --with-wx-prefix=$BUILD_DIR \
    --host=i686-w64-mingw32 \
    --with-locale
make
zip $BUILD_DIR/myrulib_$MRL_VERSION-win32.exe.zip myrulib.exe

cd $SOURCE_DIR
rm -rf build_cr3
mkdir build_cr3
cd build_cr3
LDFLAGS="-static-libgcc -static-libstdc++" ../configure \
    --host=i686-w64-mingw32 \
    --with-wx-prefix=$BUILD_DIR \
    --with-ft-prefix=$BUILD_DIR \
    --disable-freetypetest \
    --with-locale \
    --with-reader 
make
zip $BUILD_DIR/myrulib-cr_$MRL_VERSION-win32.exe.zip myrulib.exe
