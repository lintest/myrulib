#!/bin/sh
#
# Script to download and restore original lib.rus.ec MySQL database files
#
#for t in libactions libavtoraliase libavtorname libavtor libblocked libbook.old libbook libdonations libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
#

rm *.sql.gz
rm *.sql.gz.*
for t in libavtor libavtors libbook libgenre libgenremeta libgenres libjoinedbooks libmag libmags libquality librate libseq libseqs libsrclang ;
do
    n=1000; while ! wget -t0 -c http://lib.rus.ec/sql/$t.sql.gz; do if (n<0) then break; fi; n=$((n-1)); done;
    zcat $t.sql.gz | sed -e 's/USING BTREE//g' | mysql librusec -u root;
done

rm myrulib.db
php convert.php

rm myrulib.db.zip
zip myrulib.db.zip myrulib.db
