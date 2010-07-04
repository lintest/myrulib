#!/bin/sh
#
# Script to download and restore original lib.rus.ec MySQL database files
#
#for t in libactions libavtoraliase libavtorname libavtor libblocked libbook.old libbook libdonations libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
#

#cd /home/lintest/librusec/
#rm *.sql.gz
#rm *.sql.gz.*
#for t in libavtoraliase libavtorname libavtor libbook libfilename libgenrelist libgenre libjoinedbooks libseqname libseq libsrclang libtranslator;
#do
#    wget -c http://lib.rus.ec/sql/lib.$t.sql.gz;
#    zcat lib.$t.sql.gz | sed -e 's/USING BTREE//g' | mysql $DBNAME -h 127.0.0.1 -u root -D lintest_genlib -p11111111;
#done



#rm backup-last.rar
#wget -c http://free-books.dontexist.com/dailyupdated/My%20Dropbox/Public/backup-last.rar

unrar p -inul backup-last.rar | sed -e 's/USING BTREE//g' | mysql $DBNAME -h 127.0.0.1 -D lintest_genlib -u root -p11111111;

#rm myrulib.db
#php -c /home/lintest/etc/ convert.php

#rm myrulib.db.zip
#zip myrulib.db.zip myrulib.db

#cp myrulib.db.zip /home/lintest/lintest.ru/docs/myrulib/current/