<?php

require_once '../Common/datafile.php';
require_once '../Common/genres.php';
require_once '../Common/strutils.php';

require_once 'conv_info.php';

function convert_authors($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM authors");

  $sqlite_db->query("INSERT INTO authors (id, letter, full_name) VALUES(0,'#','(без автора)')");

  $sqltest = "
	SELECT libavtorname.AvtorId, libavtorname.FirstName, libavtorname.LastName, libavtorname.MiddleName, COUNT(libavtor.BookId) as Number
	FROM libavtorname INNER JOIN (
	  SELECT DISTINCT libavtor.AvtorId, libavtor.BookId 
	  FROM libavtor INNER JOIN libbook ON libbook.BookId=libavtor.BookId AND libbook.Deleted<>1 
	) AS libavtor ON libavtorname.AvtorId=libavtor.AvtorId 
    WHERE libavtorname.AvtorId>$min
	GROUP BY libavtorname.AvtorId, libavtorname.FirstName, libavtorname.LastName, libavtorname.MiddleName
  ";

  $char_list = 'А Б В Г Д Е Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ы Э Ю Я A B C D E F G H I J K L M N O P Q R S T U V W X Y Z';

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    $full_name = trim($row['LastName'])." ".trim($row['FirstName'])." ".trim($row['MiddleName']);
    $full_name = str_replace("  ", " ", $full_name);
    $search_name = strtolowerEx($full_name);
    $letter = utf8_substr($full_name,0,1);
    $letter = strtoupperEx($letter,0,1);

    if (strpos($char_list, $letter) === false) { $letter = "#"; };

    echo "Auth: ".$row['AvtorId']." - ".$letter." - ".$full_name." - ".$search_name."\n";

    $sql = "INSERT INTO authors (id, number, letter, full_name, search_name, first_name, middle_name, last_name) VALUES(?,?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['AvtorId'], $row['Number'], $letter, $full_name, $search_name, trim($row['FirstName']), trim($row['MiddleName']), trim($row['LastName'])));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}  

function convert_genres($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");
  $sqlite_db->query("DELETE FROM genres");

  $sqltext = "
	SELECT DISTINCT libgenre.BookId, GenreCode 
	FROM libgenre 
		LEFT JOIN libgenrelist ON libgenre.GenreId = libgenrelist.GenreId 
		INNER JOIN libbook ON libbook.BookId=libgenre.BookId AND libbook.Deleted<>1 
	WHERE libgenre.BookId>$min
	ORDER BY GenreCode
  ";
  
  $query = $mysql_db->query($sqltext);
  while ($row = $query->fetch_array()) {
    $genre = GenreCode($row['GenreCode']);
    echo "Genr: ".$row['GenreCode']." - ".$row['BookId']." - ".$genre."\n";
	if (!empty($genre)) {
		$sql = "INSERT INTO genres(id_book, id_genre) VALUES(?,?)";
		$insert = $sqlite_db->prepare($sql);
		$err = $insert->execute(array($row['BookId'], $genre));
	}
  }
  $sqlite_db->query("commit;");
}

function convert_books($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM books");
  $sqlite_db->query("DELETE FROM words");

  $sqltest = "
    SELECT 
      libbook.BookId, FileSize, Title, Deleted, FileType, md5, DATE_FORMAT(libbook.Time,'%y%m%d') as Time, Lang, Year,
      CASE WHEN AvtorId IS NULL THEN 0 ELSE AvtorId END AS AvtorId,
      CASE WHEN libfilename.FileName IS NULL THEN 
        CASE WHEN oldfilename.FileName IS NULL THEN CONCAT(libbook.BookId, '.', libbook.FileType) ELSE oldfilename.FileName END
        ELSE libfilename.FileName
      END AS FileName
    FROM libbook 
      LEFT JOIN libavtor ON libbook.BookId = libavtor.BookId
      LEFT JOIN libfilename ON libbook.BookId = libfilename.BookId
      LEFT JOIN oldfilename ON libbook.BookId = oldfilename.BookId
    WHERE libbook.Deleted<>1 AND libbook.BookId>$min
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo "Book: ".$row['Time']." - ".$row['BookId']." - ".$row['FileType']." - ".$row['AvtorId']." - ".$row['Title']."\n";

    $genres = "";
    $subsql = "SELECT GenreCode FROM libgenre LEFT JOIN libgenrelist ON libgenre.GenreId = libgenrelist.GenreId WHERE BookId=".$row['BookId'];
    $subquery = $mysql_db->query($subsql);
    while ($subrow = $subquery->fetch_array()) {
      $genres = $genres.GenreCode($subrow['GenreCode']);
    }
    $file_type = trim($row['FileType']);
    $file_type = trim($file_type, ".");
    $file_type = strtolower($file_type);
    $file_type = strtolowerEx($file_type);
    $lang = $row['Lang'];
    $lang = strtolower($lang);
    $lang = strtolowerEx($lang);
    $sql = "INSERT INTO books (id, id_author, title, deleted, file_name, file_size, file_type, genres, created, lang, year, md5sum) VALUES(?,?,?,?,?,?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BookId'], $row['AvtorId'], trim($row['Title']), $row['Deleted'], $row['FileName'], $row['FileSize'], $file_type, $genres, $row['Time'], $lang, $row['Year'], $row['md5']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
  
  $sqlite_db->query("commit;");
}  

function convert_dates($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM dates");

  $sqltest = "
    SELECT DATE_FORMAT(Time,'%y%m%d') as Time, MAX(BookId) as Max, MIN(BookId) as Min, COUNT(BookId) AS Num
    FROM libbook 
	WHERE Deleted<>1 AND libbook.BookId>$min
	GROUP BY DATE_FORMAT(libbook.Time,'%y%m%d')
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
	echo "Date: ".$row['Time']." - ".$row['Max']." - ".$row['Min']."\n";
    $sql = "INSERT INTO dates (id, lib_max, lib_min, lib_num) VALUES(?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['Time'], $row['Max'], $row['Min'], $row['Num']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
  $sqlite_db->query("commit;");
}  

function convert_seqnames($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM sequences");

  $sqltest = "
	SELECT libseqname.SeqId, libseqname.SeqName, COUNT(libseq.BookId) as Number
	FROM libseqname INNER JOIN (
	  SELECT DISTINCT libseq.SeqId, libseq.BookId 
	  FROM libseq INNER JOIN libbook ON libbook.BookId=libseq.BookId AND libbook.Deleted<>1 
	) AS libseq ON libseqname.SeqId=libseq.SeqId AND libseq.SeqId<>0
	WHERE libseq.SeqId>$min
	GROUP BY libseqname.SeqId, libseqname.SeqName
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo "Seqn: ".$row['SeqId']." - ".$row['SeqName']."\n";
    $sql = "INSERT INTO sequences (id, number, value) VALUES(?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['SeqId'], $row['Number'], trim($row['SeqName'])));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function convert_sequences($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM bookseq");

  $sqltest = "
    SELECT libseq.BookId, libseq.SeqId, libseq.SeqNumb, libseq.Level
    FROM libseq 
	  INNER JOIN libbook ON libseq.BookId = libbook.BookId AND libbook.Deleted<>1
	WHERE libseq.BookId>$min
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo "BkSq: ".$row['SeqId']." - ".$row['BookId']."\n";
    $sql = "INSERT INTO bookseq(id_book, id_seq, number, level) VALUES(?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BookId'], $row['SeqId'], $row['SeqNumb'], $row['Level']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function setup_params($sqlite_db, $date, $type)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (1,  'Flibusta library');");
  $sqlite_db->query("INSERT INTO params(id,value) VALUES (2,  1);");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (3,  'FLIBUSTA');");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (11, 'flibusta.net');");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (15, '$type');");
  $sqlite_db->query("INSERT INTO params(id,value) VALUES (16, $date);");
  
  $sqlite_db->query("commit;");
}

function FullImport($mysql_db, $file, $date)
{
  $sqlite_db = new PDO('sqlite:./'.$file);
  
  create_tables($sqlite_db);
  setup_params($sqlite_db, $date, "FULL");

  convert_authors($mysql_db, $sqlite_db, 0);
  convert_seqnames($mysql_db, $sqlite_db, 0);
  convert_books($mysql_db, $sqlite_db, 0);
  convert_sequences($mysql_db, $sqlite_db, 0);
  convert_genres($mysql_db, $sqlite_db, 0);
  convert_dates($mysql_db, $sqlite_db, 0);
  
  create_indexes($sqlite_db);
}

function DeltaImport($mysql_db, $date)
{
  $mysql_db->query("CREATE TABLE myrulib_update(date integer primary key, aid integer, bid integer, sid integer)");

  $sqltest = "SELECT date, aid, bid, sid FROM myrulib_update WHERE date=(SELECT MAX(date) FROM myrulib_update WHERE date<$date)";

  $query = $mysql_db->query($sqltest);
  if ($row = $query->fetch_array()) {
	
	$code = $row["date"];
	$sqlite_db = new PDO("sqlite:./$code.upd");
	
	create_tables($sqlite_db, $date);
	setup_params($sqlite_db, $date, "DELTA");
	
	convert_authors($mysql_db, $sqlite_db, $row['aid']);
	convert_seqnames($mysql_db, $sqlite_db, $row['sid']);
	convert_books($mysql_db, $sqlite_db, $row['bid']);
	convert_sequences($mysql_db, $sqlite_db, $row['bid']);
	convert_genres($mysql_db, $sqlite_db, $row['bid']);
	convert_dates($mysql_db, $sqlite_db, $row['bid']);

	author_info($mysql_db, $sqlite_db, $row['aid']);
	book_info($mysql_db, $sqlite_db, $row['bid']);

	system("zip $code.zip $code.upd");
  }
  
  $mysql_db->query("INSERT INTO myrulib_update(date) VALUES(".$date.")");

  $mysql_db->query("
	UPDATE myrulib_update SET 
	  aid=(SELECT MAX(AvtorId) FROM libavtorname),
	  bid=(SELECT MAX(BookId) FROM libbook),
	  sid=(SELECT MAX(SeqId) FROM libseq)
	WHERE date=$date");
}

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '';
$mysql_base = 'flibusta';
$sqlitefile = 'myrulib.db';

include('settings.php');

$sqlite_db = new PDO('sqlite:'.$sqlitefile);
$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

$date = date('Ymd');
echo "Today: ".$date."\n";

FullImport($mysql_db, $sqlitefile, $date);
system("zip flibusta.db.zip $file");

author_info($mysql_db, $sqlite_db, 0);
book_info($mysql_db, $sqlite_db, 0);
system("zip flibusta.full.zip $sqlitefile");

DeltaImport($mysql_db, $date);

?>
