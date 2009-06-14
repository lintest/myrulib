for t in libactions libavtoraliase libavtorname libavtor libbook libdonations libgenrelist libgenre libpolka libseq;
do
wget http://lib.rus.ec/sql/lib.$t.sql.gz;
zcat lib.$t.sql.gz | mysql $DBNAME;
done