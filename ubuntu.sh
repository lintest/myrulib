#!/bin/sh

LIST="precise oneiric natty maverick lucid"

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
