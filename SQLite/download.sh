#!/bin/sh
#
# Script to download and restore original lib.rus.ec MySQL database files
#
#for t in libactions libavtoraliase libavtorname libavtor libblocked libbook.old libbook libdonations libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
#
for t in libavtoraliase libavtorname libavtor libbook libfilename libgenrelist libgenre libjoinedbooks libpolka libseqname libseq libsrclang libtranslator;
do
wget http://lib.rus.ec/sql/lib.$t.sql.gz;
zcat lib.$t.sql.gz | mysql $DBNAME -u root lib;
done



