<?php

require_once 'genres.php';

function convert_Auth($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM Auth");

  $sqlite_db->query("INSERT INTO A(aid, Letter) VALUES(0,'#')");

  $sqltest = "
	SELECT libavtorname.AvtorId, libavtorname.FirstName, libavtorname.LastName, libavtorname.MiddleName, COUNT(DISTINCT libavtor.BookId) as Number
	FROM libavtorname 
      INNER JOIN libavtor ON libavtorname.AvtorId=libavtor.AvtorId 
      INNER JOIN libbook ON libbook.BookId=libavtor.BookId AND libbook.Deleted<>1 
	GROUP BY libavtorname.AvtorId, libavtorname.FirstName, libavtorname.LastName, libavtorname.MiddleName
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['AvtorId']." - ".$row['FirstName']." ".$row['MiddleName']." ".$row['LastName']."\n";
    $sql = "INSERT INTO a(aid, Numb, First, Middle, Last) VALUES(?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    $insert->execute(array($row['AvtorId'], $row['Number'], trim($row['FirstName']), trim($row['MiddleName']), trim($row['LastName'])));
    $insert->closeCursor();
  }
  $query->close(); 
  $sqlite_db->query("commit;");
}  

function convert_Book($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM Book");

  $sqltest = "
    SELECT 
      libbook.BookId, FileSize, Title, FileType, md5, DATE_FORMAT(libbook.Time,'%y%m%d') as Time, Lang, Year,
      CASE WHEN libfilename.FileName IS NULL THEN 
        CASE WHEN oldfilename.FileName IS NULL THEN CONCAT(libbook.BookId, '.', libbook.FileType) ELSE oldfilename.FileName END
        ELSE libfilename.FileName
      END AS FileName
    FROM libbook 
      LEFT JOIN libfilename ON libbook.BookId = libfilename.BookId
      LEFT JOIN oldfilename ON libbook.BookId = oldfilename.BookId
    WHERE libbook.Deleted<>1
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
	
    $book = $row['BookId'];
    echo $row['Time']." - ".$book." - ".$row['FileType']." - ".$row['Title']."\n";

    $genres = "";
    $subsql = "SELECT GenreCode FROM libgenre LEFT JOIN libgenrelist ON libgenre.GenreId = libgenrelist.GenreId WHERE BookId=".$row['BookId'];
    $subquery = $mysql_db->query($subsql);
    while ($subrow = $subquery->fetch_array()) {
      $genr = genreCode($subrow['GenreCode']);
      $genres = $genres.$genr;
      $sql = "INSERT INTO bg(bid, gid) VALUES(?,?)";
      $insert = $sqlite_db->prepare($sql);
      $insert->execute(array($book, $genres));
      $insert->closeCursor();
    }
    $subquery->close();
	
    $file_type = trim($row['FileType']);
    $file_type = trim($file_type, ".");
    $file_type = strtolower($file_type);
    $file_type = strtolowerEx($file_type);
    $lang = $row['Lang'];
    $lang = strtolower($lang);
    $lang = strtolowerEx($lang);
    $sql = "INSERT INTO b(bid, Title, File, Size, Type, Genr, Date, Lang, Year, Md5s) VALUES(?,?,?,?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    $insert->execute(array($book, trim($row['Title']), $row['FileName'], $row['FileSize'], $file_type, $genres, $row['Time'], $lang, $row['Year'], $row['md5']));
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}  

function convert_Seqn($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM Seqn");

  $sqltest = "
	SELECT libseqname.SeqId, libseqname.SeqName, COUNT(libseq.BookId) as Number
	FROM libseqname INNER JOIN (
	  SELECT DISTINCT libseq.SeqId, libseq.BookId 
	  FROM libseq INNER JOIN libbook ON libbook.BookId=libseq.BookId AND libbook.Deleted<>1 
	) AS libseq ON libseqname.SeqId=libseq.SeqId AND libseq.SeqId<>0
	GROUP BY libseqname.SeqId, libseqname.SeqName
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['SeqId']." - ".$row['SeqName']."\n";
    $sql = "INSERT INTO s(sid, Numb, Seqn) VALUES(?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    $insert->execute(array($row['SeqId'], $row['Number'], trim($row['SeqName'])));
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}

function convert_BkAuth($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM ba");

  $sqltest = "
    SELECT DISTINCT
      libbook.BookId, 
	  CASE WHEN libavtor.AvtorId IS NULL THEN 0 ELSE libavtor.AvtorId END AS AvtorId
    FROM libbook 
      LEFT JOIN libavtor ON libbook.BookId = libavtor.BookId
	WHERE libbook.Deleted<>1
  ";
  
  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['AvtorId']." - ".$row['BookId']."\n";
    $sql = "INSERT INTO ba(bid, aid) VALUES(?,?)";
    $insert = $sqlite_db->prepare($sql);
    $insert->execute(array($row['BookId'], $row['AvtorId']));
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}

function convert_BkSeqn($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM bs");

  $sqltest = "
    SELECT DISTINCT
      libbook.BookId, 
	  CASE WHEN libseq.SeqId IS NULL THEN 0 ELSE libseq.SeqId END AS SeqId,
	  libseq.SeqNumb, libseq.Level
    FROM libbook 
      LEFT JOIN libseq ON libbook.BookId = libseq.BookId
	WHERE libbook.Deleted<>1
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['SeqId']." - ".$row['BookId']."\n";
    $sql = "INSERT INTO bs(bid, sid, Num) VALUES(?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    $insert->execute(array($row['BookId'], $row['SeqId'], $row['SeqNumb']));
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}

function create_tables($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("
	CREATE TABLE a(
	  Aid integer primary key,
	  Letter char(1),
	  First varchar(128),
	  Middle varchar(128),
	  Last varchar(128),
	  Find varchar(255),
	  Full varchar(255),
	  Numb integer,
	  Dscr text);
  ");

  $sqlite_db->query("
	CREATE TABLE b(
	  Bid integer primary key,
	  Zid integer,
	  Title varchar(255) not null,
	  File text,
	  Path text,
	  Size integer,
	  Type text,
	  Md5s text,
	  Genr text,
	  Lang text,
	  Date integer,
	  Year integer,
	  Dscr text);
  ");
  
  $sqlite_db->query("
	CREATE TABLE d(
	  Date integer primary key,
	  Numb integer);
  ");

  $sqlite_db->query("
	CREATE TABLE s(
	  Sid integer primary key,
	  Seqn text,
	  Numb integer);
  ");

  $sqlite_db->query("
	CREATE TABLE z(
	  Zid integer primary key,
	  File text,
	  Path text,
	  Size integer,
	  Numb integer);
  ");
  
  $sqlite_db->query("
	CREATE TABLE vz(
	  vid integer primary key,
	  File text);
  ");
  
  $sqlite_db->query("
	CREATE TABLE ve(
	  vid integer,
	  Entry text);
  ");
  
  $sqlite_db->query("
	CREATE TABLE ba(
	  Bid integer not null,
	  Aid integer);
  ");
  
  $sqlite_db->query("
	CREATE TABLE bf(
	  Bid integer not null,
	  Zid integer,
	  File text,
	  Path text);
  ");
  
  $sqlite_db->query("
	CREATE TABLE bg(
	  Bid integer not null,
	  Gid text);
  ");

  $sqlite_db->query("
	CREATE TABLE bi(
	  Bid integer not null,
	  Isbn text);
  ");

  $sqlite_db->query("
	CREATE TABLE bp(
	  Bid integer not null,
	  Pict text);
  ");

  $sqlite_db->query("
	CREATE TABLE bs(
	  Bid integer not null,
	  Sid integer,
	  Num integer);
  ");

  $sqlite_db->query("CREATE TABLE params(id integer primary key, value integer, text text);");
  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES ( 1, 'Flibusta library');");
  $sqlite_db->query("INSERT INTO params(id,value) VALUES ( 2, 20);");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES ( 3, 'FLIBUSTA');");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (11, 'flibusta.net');");

  $sqlite_db->query("commit;");
}

function convert_count($sqlite_db)
{
  $sqlite_db->query("begin transaction;");
  
  $sqlite_db->query("UPDATE a SET Numb=(SELECT COUNT(bid) FROM ba WHERE ba.aid=a.aid)");
  $sqlite_db->query("UPDATE s SET Numb=(SELECT COUNT(bid) FROM bs WHERE bs.sid=s.sid)");
  
  $sqlite_db->query("commit;");
}

function create_indexes($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("CREATE INDEX A_Letter ON a(Letter);");
  $sqlite_db->query("CREATE INDEX A_Find ON a(Find);");

  $sqlite_db->query("CREATE INDEX B_Title ON b(Title);");
  $sqlite_db->query("CREATE INDEX B_Md5s ON b(Md5s);");
  $sqlite_db->query("CREATE INDEX B_Date ON b(Date);");

  $sqlite_db->query("CREATE INDEX S_Seqn ON s(Seqn);");

  $sqlite_db->query("CREATE INDEX BA_Bid ON ba(Bid);");
  $sqlite_db->query("CREATE INDEX BA_Aid ON ba(Aid);");

  $sqlite_db->query("CREATE INDEX BF_Bid ON bf(Bid);");
  $sqlite_db->query("CREATE INDEX BF_Zid ON bf(Zid);");

  $sqlite_db->query("CREATE INDEX BG_Bid ON bg(Bid);");
  $sqlite_db->query("CREATE INDEX BG_Gid ON bg(Gid);");

  $sqlite_db->query("CREATE INDEX BI_Bid ON bi(Bid);");
  $sqlite_db->query("CREATE INDEX BI_Isbn ON bi(Isbn);");

  $sqlite_db->query("CREATE INDEX BS_Bid ON bs(Bid);");
  $sqlite_db->query("CREATE INDEX BS_Sid ON bs(Sid);");
  
  $sqlite_db->query("CREATE INDEX VE_Vid ON ve(Vid);");

  $sqlite_db->query("commit;");

  $sqlite_db->query("vacuum;");
}

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '';
$mysql_base = 'flibusta';
$sqlitefile = './flibusta.db';

include('settings.php');

$sqlite_db = new PDO('sqlite:'.$sqlitefile);
$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

create_tables($sqlite_db);

convert_Auth($mysql_db, $sqlite_db);
convert_Book($mysql_db, $sqlite_db);
convert_Seqn($mysql_db, $sqlite_db);
convert_BkAuth($mysql_db, $sqlite_db);
convert_BkSeqn($mysql_db, $sqlite_db);

?>
