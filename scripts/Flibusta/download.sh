#!/bin/sh
#
# Script to download and restore original lib.rus.ec MySQL database files
#
#for t in libactions libavtoraliase libavtorname libavtor libblocked libbook.old libbook libdonations libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
#

rm *.sql.gz
for t in libavtor libtranslator libavtoraliase libavtorname libbook libfilename libgenre libgenrelist libjoinedbooks librate libseqname libseq libsrclang reviews b.annotations b.annotations_pics a.annotations a.annotations_pics;

do
    n=1000; while ! wget -t0 -c http://flibusta.net/sql/lib.$t.sql.gz; do if (let "$n<0") then break; fi; sleep 10s; n=$((n-1)); done;
    zcat lib.$t.sql.gz | sed -e 's/USING BTREE//g' | mysql flibusta -u root
done

rm myrulib.db
php conv_book.php

rm flibusta.db.zip
zip flibusta.db.zip myrulib.db

php conv_info.php

rm flibusta.db.full.zip
zip flibusta.db.full.zip myrulib.db
