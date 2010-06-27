#!/bin/sh
#
# Script to download and restore original lib.rus.ec MySQL database files
#
#for t in libactions libavtoraliase libavtorname libavtor libblocked libbook.old libbook libdonations libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
#

rm *.sql.gz
for t in libavtor libtranslator libavtoraliase libavtorname libbook libfilename libgenre libgenrelist libjoinedbooks librate libseqname libseq libsrclang reviews b.annotations b.annotations_pics a.annotations a.annotations_pics;

do
    wget http://flibusta.net/sql/lib.$t.sql.gz;
    zcat lib.$t.sql.gz | sed -e 's/USING BTREE//g' | mysql flibusta -u root
done

rm myrulib.db
php conv_book.php

rm flibusta.db.zip
zip flibusta.db.zip myrulib.db

php conv_info.php

rm flibusta.db.full.zip
zip flibusta.db.full.zip myrulib.db
