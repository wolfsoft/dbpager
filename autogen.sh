#!/bin/sh


# Script for regenerating all autogenerated files.
echo Regenerating all dbPager autogenerated files...
if [ -f /usr/local/share/aclocal/libtool.m4 ]; then
cp /usr/local/share/aclocal/libtool.m4 ./aclocal.m4
else
if [ -f /usr/share/aclocal/libtool.m4 ]; then
cp /usr/share/aclocal/libtool.m4 ./aclocal.m4
else
echo "You should have libtool package installed."
exit
fi
fi

autopoint -f
libtoolize -c -f
aclocal -I m4
autoheader &&
automake --gnu -c -a &&
autoconf &&
autoheader

# Regenerate language files for gettext
if [ -f po/POTFILES.in ]; then
rm -f po/POTFILES.in
fi
touch po/POTFILES.in
find . -name '*.cpp' >>po/POTFILES.in
find . -name '*.h' >>po/POTFILES.in
xgettext -k_ -f po/POTFILES.in -C -F -o dbpager.pot -p po
cd po
rm -rf LINGUAS
touch LINGUAS
if [ -f *.po ]; then
for f in *.po; do
  lang=`echo $f | sed -e 's,\.po$,,'`
  msgmerge -U $lang.po dbpager.pot
  msgfmt -c -o $lang.gmo $lang.po
  echo $lang >>LINGUAS
done
fi
cd ..

# Regenerate subproject build files
echo Regenerating all dbPager subprojects autogenerated files...
echo mimetic...
cd src/mimetic-0.9.7

if [ -f /usr/local/share/aclocal/libtool.m4 ]; then
cp /usr/local/share/aclocal/libtool.m4 ./aclocal.m4
else
if [ -f /usr/share/aclocal/libtool.m4 ]; then
cp /usr/share/aclocal/libtool.m4 ./aclocal.m4
else
echo "You should have libtool package installed."
exit
fi
fi

libtoolize -c -f
aclocal -I m4
autoheader &&
automake --gnu -c -a &&
autoconf &&
autoheader &&
autoreconf -f -i

cd ../..

echo fcgi...
cd src/fcgi-2.4.1-SNAP-0910052249

if [ -f /usr/local/share/aclocal/libtool.m4 ]; then
cp /usr/local/share/aclocal/libtool.m4 ./aclocal.m4
else
if [ -f /usr/share/aclocal/libtool.m4 ]; then
cp /usr/share/aclocal/libtool.m4 ./aclocal.m4
else
echo "You should have libtool package installed."
exit
fi
fi

libtoolize -c -f
aclocal -I m4
autoheader &&
automake --gnu -c -a &&
autoconf &&
autoheader &&
autoreconf -f -i

cd ../..

