#!/bin/sh

RELTAG=0.3-devel

rm -rf srcdocs-ru 

doxygen doxy-ru.conf
tar cf - srcdocs-ru | gzip -9 >srcdocs-$RELTAG-ru.tar.gz
tar cf - srcdocs-ru | bzip2 -9 >srcdocs-$RELTAG-ru.tar.bz2
zip -q -9 -r srcdocs-$RELTAG-ru.zip srcdocs-ru

rm -rf srcdocs-en 

doxygen doxy-en.conf
tar cf - srcdocs-en | gzip -9 >srcdocs-$RELTAG-en.tar.gz
tar cf - srcdocs-en | bzip2 -9 >srcdocs-$RELTAG-en.tar.bz2
zip -q -9 -r srcdocs-$RELTAG-en.zip srcdocs-en

ls -l