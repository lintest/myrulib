#!/bin/sh
for t in libactions libavtoraliase libavtorname libavtor libblocked libbook.old libbook libdonations libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
do
wget http://rus.ec/sql/$t;
zcat $t | mysql $DBNAME -u root lib;
done



