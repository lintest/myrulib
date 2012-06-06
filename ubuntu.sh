#!/bin/sh

LIST="quantal precise oneiric natty"

NUMBER=1

VERSION=`cat configure | grep PACKAGE_VERSION= | sed "s/.*=//" | sed "s/'//g"`  

echo 
echo "============================================================"
echo 

mkdir tarball
cd tarball

cd myrulib-$VERSION

rm -rf debian
mkdir debian
mkdir debian/source

for FILENAME in changelog compat copyright dirs docs menu;
do
  cp ../../debian/$FILENAME debian
done

cp ../../debian/source/format debian/source
cp ../../debian/myrulib/control debian
cp ../../debian/myrulib/rules debian

for DISTRIB in $LIST;
do
  echo "myrulib ($VERSION-$DISTRIB$NUMBER) $DISTRIB; urgency=low" > debian/changelog
  cat ../../debian/changelog | sed '1d'>> debian/changelog
  debuild -S -sa
done

cd ..

echo 
echo "============================================================"
echo 

cd myrulib-$VERSION

echo "myrulib ($VERSION-squeeze$NUMBER) stable; urgency=low" > debian/changelog
cat ../../debian/changelog | sed '1d'>> debian/changelog

debuild -S -sa

mkdir ../osc_myrulib
mv ../*squeeze* ../osc_myrulib
cp ../myrulib_$VERSION.orig.tar.bz2 ../osc_myrulib

echo "Source: myrulib" > debian/control
cat ../../debian/myrulib-cr/control | sed '1d'>> debian/control
cp ../../debian/myrulib-cr/rules debian

debuild -S -sa

mkdir ../osc_myrulib-cr
cp ../*squeeze* ../osc_myrulib-cr
cp ../myrulib_$VERSION.orig.tar.bz2 ../osc_myrulib-cr

cd ..

echo 
echo "============================================================"
echo 

cd myrulib-cr-$VERSION

rm -rf debian
mkdir debian
mkdir debian/source

for FILENAME in changelog compat copyright dirs docs menu;
do
  cp ../../debian/$FILENAME debian
done

cp ../../debian/source/format debian/source
cp ../../debian/myrulib-cr/control debian
cp ../../debian/myrulib-cr/rules debian

for DISTRIB in $LIST;
do
  echo "myrulib-cr ($VERSION-$DISTRIB$NUMBER) $DISTRIB; urgency=low" > debian/changelog
  cat ../../debian/changelog | sed '1d'>> debian/changelog
  debuild -S -sa
done
