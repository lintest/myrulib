<?php

require_once 'bbcode/bbcode.lib.php';

function strtolowerEx($str){
 $result = $str;
 global $strtolowerEx_pairs;
 if(!isset($strtolowerEx_pairs)){
  $from = 'А Б В Г Д Е Ё Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я A B C D E F G H I J K L M N O P Q R S T U V W X Y Z';
  $to =   'а б в г д е ё ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я a b c d e f g h i j k l m n o p q r s t u v w x y z';
  $from = explode(' ', trim($from));
  $to = explode(' ', trim($to));
  $cfrom = count($from);
  $cto = count($to);
  $count = $cfrom > $cto ? $cto : $cfrom;
  for($i = 0; $i < $count; $i++){$strtolowerEx_pairs[$from[$i]] = $to[$i];}
 }
 $result = strtr($str, $strtolowerEx_pairs);
 return $result;
}

function utf8_strlen($s)
{
    return preg_match_all('/./u', $s, $tmp);
}

function genreCode($s)
{
 global $genreArray;
  if(!isset($genreArray)){
    $genreArray = array(
      "sf_history" => "11",
      "sf_action" => "12",
      "sf_epic" => "13",
      "sf_heroic" => "14",
      "sf_detective" => "15",
      "sf_cyberpunk" => "16",
      "sf_space" => "17",
      "sf_social" => "18",
      "sf_horror" => "19",
      "sf_humor" => "1A",
      "sf_fantasy" => "1B",
      "sf" => "10",
      "det_classic" => "21",
      "det_police" => "22",
      "det_action" => "23",
      "det_irony" => "24",
      "det_history" => "25",
      "det_espionage" => "26",
      "det_crime" => "27",
      "det_political" => "28",
      "det_maniac" => "29",
      "det_hard" => "2A",
      "thriller" => "2B",
      "detective" => "20",
      "prose_classic" => "31",
      "prose_history" => "32",
      "prose_contemporary" => "33",
      "prose_counter" => "34",
      "prose_rus_classic" => "35",
      "prose_su_classics" => "36",
      "love_contemporary" => "41",
      "love_history" => "42",
      "love_detective" => "43",
      "love_short" => "44",
      "love_erotica" => "45",
      "adv_western" => "51",
      "adv_history" => "52",
      "adv_indian" => "53",
      "adv_maritime" => "54",
      "adv_geo" => "55",
      "adv_animal" => "56",
      "adventure" => "50",
      "child_tale" => "61",
      "child_verse" => "62",
      "child_prose" => "63",
      "child_sf" => "64",
      "child_det" => "65",
      "child_adv" => "66",
      "child_education" => "67",
      "children" => "60",
      "poetry" => "71",
      "dramaturgy" => "72",
      "antique_ant" => "81",
      "antique_european" => "82",
      "antique_russian" => "83",
      "antique_east" => "84",
      "antique_myths" => "85",
      "antique" => "80",
      "sci_history" => "91",
      "sci_psychology" => "92",
      "sci_culture" => "93",
      "sci_religion" => "94",
      "sci_philosophy" => "95",
      "sci_politics" => "96",
      "sci_business" => "97",
      "sci_juris" => "98",
      "sci_linguistic" => "99",
      "sci_medicine" => "9A",
      "sci_phys" => "9B",
      "sci_math" => "9C",
      "sci_chem" => "9D",
      "sci_biology" => "9E",
      "sci_tech" => "9F",
      "science" => "90",
      "comp_www" => "A1",
      "comp_programming" => "A2",
      "comp_hard" => "A3",
      "comp_soft" => "A4",
      "comp_db" => "A5",
      "comp_osnet" => "A6",
      "computers" => "A0",
      "ref_encyc" => "B1",
      "ref_dict" => "B2",
      "ref_ref" => "B3",
      "ref_guide" => "B4",
      "reference" => "B0",
      "nonf_biography" => "C1",
      "nonf_publicism" => "C2",
      "nonf_criticism" => "C3",
      "design" => "C4",
      "nonfiction" => "C5",
      "religion_rel" => "D1",
      "religion_esoterics" => "D2",
      "religion_self" => "D3",
      "religion" => "D0",
      "humor_anecdote" => "E1",
      "humor_prose" => "E2",
      "humor_verse" => "E3",
      "humor" => "E0",
      "home_cooking" => "F1",
      "home_pets" => "F2",
      "home_crafts" => "F3",
      "home_entertain" => "F4",
      "home_health" => "F5",
      "home_garden" => "F6",
      "home_diy" => "F7",
      "home_sport" => "F8",
      "home_sex" => "F9",
      "home" => "F0",
    );
  }
  return $genreArray[$s];
}

function strtoupperEx($str){
 $result = $str;
 global $strtoupperEx_pairs;
 if(!isset($strtoupperEx_pairs)){
  $from = 'а б в г д е ё ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я a b c d e f g h i j k l m n o p q r s t u v w x y z Ё';
  $to =   'А Б В Г Д Е Е Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я A B C D E F G H I J K L M N O P Q R S T U V W X Y Z Е';
  $from = explode(' ', trim($from));
  $to = explode(' ', trim($to));
  $cfrom = count($from);
  $cto = count($to);
  $count = $cfrom > $cto ? $cto : $cfrom;
  for($i = 0; $i < $count; $i++){$strtoupperEx_pairs[$from[$i]] = $to[$i];}
 }
 $result = strtr($str, $strtoupperEx_pairs);
 return $result;
}

function utf8_substr($s, $offset, $len = 'all')
{
    if ($offset<0) $offset = utf8_strlen($s) + $offset;
    if ($len!='all')
    {
        if ($len<0) $len = utf8_strlen($s) - $offset + $len;
        $xlen = utf8_strlen($s) - $offset;
        $len = ($len>$xlen) ? $xlen : $len;
        preg_match('/^.{' . $offset . '}(.{0,'.$len.'})/us', $s, $tmp);
    }
    else
    {
        preg_match('/^.{' . $offset . '}(.*)/us', $s, $tmp);
    }
    return (isset($tmp[1])) ? $tmp[1] : false;
}

function convert_Auth($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM Auth");

  $sqlite_db->query("INSERT INTO Auth (AuthId, Letter, FullName) VALUES(0,'#','(без автора)')");

  $sqltest = "
	SELECT libavtorname.AvtorId, libavtorname.FirstName, libavtorname.LastName, libavtorname.MiddleName, COUNT(libavtor.BookId) as Number
	FROM libavtorname INNER JOIN (
	  SELECT DISTINCT libavtor.AvtorId, libavtor.BookId 
	  FROM libavtor INNER JOIN libbook ON libbook.BookId=libavtor.BookId AND libbook.Deleted<>1 
	) AS libavtor ON libavtorname.AvtorId=libavtor.AvtorId 
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

    echo $row['AvtorId']." - ".$letter." - ".$full_name." - ".$search_name."\n";

    $sql = "INSERT INTO Auth (AuthId, BookCount, Letter, FullName, SearchName, FirstName, MiddleName, LastName) VALUES(?,?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['AvtorId'], $row['Number'], $letter, $full_name, $search_name, trim($row['FirstName']), trim($row['MiddleName']), trim($row['LastName'])));
    if($err === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
  $query->close(); 
  $sqlite_db->query("commit;");
}  

function info_auth($mysql_db, $sqlite_db)
{
  $bb = new bbcode;
  $bb->autolinks = false;
  $sqltest = "SELECT * FROM libaannotations";
  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['Title']."\n";
    $sql = "UPDATE Auth SET Descr=? where AuthId=?";
    $insert = $sqlite_db->prepare($sql);
    $bb -> parse($row['Body']);
    $body = $bb->get_html();
    $body = str_replace("&lt;", "<", $body);
    $body = str_replace("&gt;", ">", $body);
    $insert->execute(array($body, $row['AvtorId']));
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

	$authors = "";
    $subsql = "SELECT DISTINCT libavtor.AvtorId FROM libavtor WHERE libavtor.BookId=".$row['BookId'];
    $subquery = $mysql_db->query($subsql);
	$num = 0;
    while ($subrow = $subquery->fetch_array()) {
	  if ($authors <> "") $authors = $authors.",";
      $authors = $authors.$subrow['AvtorId'];
	  $num = 1;
    }
	$subquery->close();

	if ($num>0) echo $row['Time']." - ".$row['BookId']." - ".$authors." - ".$row['FileType']." - ".$row['Title']."\n";

    $genres = "";
    $subsql = "SELECT GenreCode FROM libgenre LEFT JOIN libgenrelist ON libgenre.GenreId = libgenrelist.GenreId WHERE BookId=".$row['BookId'];
    $subquery = $mysql_db->query($subsql);
    while ($subrow = $subquery->fetch_array()) {
      $genres = $genres.genreCode($subrow['GenreCode']);
    }
	$subquery->close();


    $file_type = trim($row['FileType']);
    $file_type = trim($file_type, ".");
    $file_type = strtolower($file_type);
    $file_type = strtolowerEx($file_type);
    $lang = $row['Lang'];
    $lang = strtolower($lang);
    $lang = strtolowerEx($lang);
    $sql = "INSERT INTO Book (BookId, Title, AuthIds, FileName, FileSize, FileType, Genres, FileDate, Lang, Year, Md5sum) VALUES(?,?,?,?,?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BookId'], trim($row['Title']), $authors, $row['FileName'], $row['FileSize'], $file_type, $genres, $row['Time'], $lang, $row['Year'], $row['md5']));
    if($err === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}  

function info_book($mysql_db, $sqlite_db)
{
  $bb = new bbcode;
  $bb->autolinks = false;
  $sqltest = "SELECT * FROM libbannotations";
  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['Title']."\n";
    $sql = "UPDATE Book SET Descr=? where BookId=?";
    $insert = $sqlite_db->prepare($sql);
    $bb -> parse($row['Body']);
	$body = $bb->get_html();
    $body = str_replace("&lt;", "<", $body);
    $body = str_replace("&gt;", ">", $body);
    $insert->execute(array($body, $row['BookId']));
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
    $sql = "INSERT INTO Seqn (SeqnId, BookCount, SeqnName) VALUES(?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['SeqId'], $row['Number'], trim($row['SeqName'])));
    if($err === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}

function convert_BkSeqn($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM BkSeqn");

  $sqltest = "
    SELECT DISTINCT
      libbook.BookId, 
	  CASE WHEN libseq.SeqId IS NULL THEN 0 ELSE libseq.SeqId END AS SeqId,
	  CASE WHEN libavtor.AvtorId IS NULL THEN 0 ELSE libavtor.AvtorId END AS AvtorId,
	  libseq.SeqNumb, libseq.Level
    FROM libbook 
      LEFT JOIN libseq ON libbook.BookId = libseq.BookId
      LEFT JOIN libavtor ON libbook.BookId = libavtor.BookId
	WHERE libbook.Deleted<>1
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['SeqId']." - ".$row['BookId']."\n";
    $sql = "INSERT INTO BkSq(BookId, SeqnId, AuthId, Number, Level) VALUES(?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    $err= $insert->execute(array($row['BookId'], $row['SeqId'], $row['AvtorId'], $row['SeqNumb'], $row['Level']));
    if($err === false){ $err= $sqlite_db->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
  $query->close(); 

  $sqlite_db->query("commit;");
}

function create_tables($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("
	CREATE TABLE Auth(
	  AuthId integer primary key,
	  Letter char(1),
	  SearchName varchar(255),
	  FullName varchar(255),
	  FirstName varchar(128),
	  MiddleName varchar(128),
	  LastName varchar(128),
	  BookCount integer,
	  Descr text);
  ");

  $sqlite_db->query("
	CREATE TABLE Book(
	  BookId integer primary key,
	  Title varchar(255) not null,
	  AuthIds text,
	  ArchId integer,
	  FileName text,
	  FilePath text,
	  FileSize integer,
	  FileType varchar(20),
	  FileDate integer,
	  Md5sum char(32),
	  Genres text,
	  Lang char(2),
	  Year integer,
	  Descr text);
  ");

  $sqlite_db->query("
	CREATE TABLE Arch(
	  ArchId integer primary key,
	  FileName text,
	  FilePath text,
	  FileSize integer,
	  FileCount integer,
	  FileType varchar(20),
	  description text);
  ");

  $sqlite_db->query("
	CREATE TABLE File(
	  BookId integer not null,
	  ArchId integer,
	  FileName text,
	  FilePath text);
  ");

  $sqlite_db->query("
	CREATE TABLE Seqn(
	  SeqnId integer primary key,
	  SeqnName text,
	  BookCount integer);
  ");

  $sqlite_db->query("
	CREATE TABLE BkSq(
	  BookId integer not null,
	  SeqnId integer,
	  AuthId integer,
	  Number integer, 
	  Level integer);
  ");

  $sqlite_db->query("CREATE TABLE params(id integer primary key, value integer, text text);");
  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(text) VALUES ('Flibusta library');");
  $sqlite_db->query("INSERT INTO params(value) VALUES (10);");
  $sqlite_db->query("INSERT INTO params(text) VALUES ('FLIBUSTA');");
  $sqlite_db->query("INSERT INTO params(id,text) VALUES (11,'flibusta.net');");

  $sqlite_db->query("commit;");
}

function create_indexes($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("CREATE INDEX Auth_Letter ON Auth(Letter);");
  $sqlite_db->query("CREATE INDEX Auth_SearchName ON Auth(SearchName);");

  $sqlite_db->query("CREATE INDEX Book_Title ON Book(Title);");
  $sqlite_db->query("CREATE INDEX Book_ArchId ON Book(ArchId);");
  $sqlite_db->query("CREATE INDEX Book_Md5sum ON Book(Md5sum);");

  $sqlite_db->query("CREATE INDEX Seqn_SeqnName ON Seqn(SeqnName);");

  $sqlite_db->query("CREATE INDEX File_BookId ON File(BookId);");
  $sqlite_db->query("CREATE INDEX File_ArchId ON File(ArchId);");

  $sqlite_db->query("CREATE INDEX BkSq_BookId ON BkSq(BookId);");
  $sqlite_db->query("CREATE INDEX BkSq_AuthId ON BkSq(AuthId,SeqnId);");

  $sqlite_db->query("commit;");

  $sqlite_db->query("vacuum;");
}

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '';
$mysql_base = 'flibusta';
$sqlitefile = './dataview.db';

include('settings.php');

$sqlite_db = new PDO('sqlite:'.$sqlitefile);
$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

create_tables($sqlite_db);

convert_Auth($mysql_db, $sqlite_db);
convert_Book($mysql_db, $sqlite_db);
convert_Seqn($mysql_db, $sqlite_db);
convert_BkSeqn($mysql_db, $sqlite_db);

info_auth($mysql_db, $sqlite_db);
info_book($mysql_db, $sqlite_db);

create_indexes($sqlite_db);

?>
