#!/bin/sh

cd srcdocs-ru
mv html srcdocs-0.3.51-ru
tar cf - srcdocs-0.3.51-ru | gzip -9 >srcdocs-0.3.51-ru.tar.gz
tar cf - srcdocs-0.3.51-ru | bzip2 -9 >srcdocs-0.3.51-ru.tar.bz2
zip -q -9 -r srcdocs-0.3.51-ru.zip srcdocs-0.3.51-ru
mv srcdocs-0.3.51-ru html

cd ..

cd srcdocs-en
mv html srcdocs-0.3.51-en
tar cf - srcdocs-0.3.51-en | gzip -9 >srcdocs-0.3.51-en.tar.gz
tar cf - srcdocs-0.3.51-en | bzip2 -9 >srcdocs-0.3.51-en.tar.bz2
zip -q -9 -r srcdocs-0.3.51-en.zip srcdocs-0.3.51-en
mv srcdocs-0.3.51-en html

cd ..

du -ha --max-depth=1 srcdocs-ru srcdocs-en 