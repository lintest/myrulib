<?php

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

function convert_authors($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM authors");

  $sqlite_db->query("INSERT INTO authors (id, letter, full_name) VALUES(0,'#','(без автора)')");

  $sqltest = "
    SELECT * FROM libavtorname
    WHERE EXISTS(SELECT AvtorId FROM libavtor WHERE libavtor.AvtorId = libavtorname.AvtorId )
  ";

  $char_list = 'А Б В Г Д Е Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ы Э Ю Я A B C D E F G H I J K L M N O P Q R S T U V W X Y Z';

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    $full_name = $row['LastName']." ".$row['FirstName']." ".$row['MiddleName'];
    $full_name = trim($full_name);
    $full_name = str_replace("  ", " ", $full_name);
    $search_name = strtolowerEx($full_name);
    $letter = utf8_substr($full_name,0,1);
    $letter = strtoupperEx($letter,0,1);

    if (strpos($char_list, $letter) === false) { $letter = "#"; };

    echo $row['AvtorId']." - ".$letter." - ".$full_name." - ".$search_name."\n";

    $sql = "INSERT INTO authors (id, letter, full_name, search_name, first_name, middle_name, last_name) VALUES(?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['AvtorId'], $letter, $full_name, $search_name, $row['FirstName'], $row['MiddleName'], $row['LastName']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function convert_books($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM books");
  $sqlite_db->query("DELETE FROM words");

  $sqltest = "
    SELECT libbook.BookId, FileSize, Title, Deleted, FileType, CASE WHEN AvtorId IS NULL THEN 0 ELSE AvtorId END AS AvtorId
    FROM libbook LEFT JOIN libavtor ON libbook.BookId = libavtor.BookId
    WHERE Deleted<>1
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    $filename = $row['BookId'].".".$row['FileType'];
    echo $row['BookId']." - ".$filename." - ".$row['AvtorId']." - ".$row['Title']."\n";

    $genres = "";
    $subsql = "SELECT GenreCode FROM libgenre LEFT JOIN libgenrelist ON libgenre.GenreId = libgenrelist.GenreId WHERE BookId=".$row['BookId'];
    $subquery = $mysql_db->query($subsql);
    while ($subrow = $subquery->fetch_array()) {
      $genres = $genres.genreCode($subrow['GenreCode']);
    }

    $sql = "INSERT INTO books (id, id_author, title, deleted, file_name, file_size, file_type, genres) VALUES(?,?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BookId'], $row['AvtorId'], $row['Title'], $row['Deleted'], $filename, $row['FileSize'], $row['FileType'], $genres));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
/*
    $words = explode(' ', trim($row['Title']));
    $cwords = count($words);
    $sql = "INSERT INTO words (word, id_book, number) VALUES(?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    for($i = 0; $i < $cwords; $i++){
      if (utf8_strlen($words[$i])<3) continue;
      if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
      $err= $insert->execute(array(strtolowerEx($words[$i]), $row['BookId'], $i));
      if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
      $insert->closeCursor();
    }
*/
  }

  $sqlite_db->query("commit;");
}

function fix_avtoraliase($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("UPDATE authors SET newid=0");

  $sqltest = "SELECT * FROM libavtoraliase ORDER BY AliaseId";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo "replace: ".$row['BadId']." - ".$row['GoodId']."\n";
    $sql = "UPDATE authors SET newid = ? WHERE id = ?";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BadId'], $row['GoodId']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function convert_seqnames($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM sequences");

  $sqltest = "SELECT * FROM libseqname";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['SeqId']." - ".$row['SeqName']."\n";
    $sql = "INSERT INTO sequences (id, value) VALUES(?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['SeqId'], $row['SeqName']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function convert_sequences($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM bookseq");

  $sqltest = "
    SELECT libseq.BookId, libseq.SeqId, libseq.SeqNumb, libseq.Level, libavtor.AvtorId
    FROM libseq INNER JOIN libavtor ON libseq.BookId = libavtor.BookId
    WHERE NOT libseq.BookId IN (SELECT BookId FROM libbook WHERE Deleted=1)
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['SeqId']." - ".$row['BookId']."\n";
    $sql = "INSERT INTO bookseq(id_book, id_seq, number, level, id_author) VALUES(?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BookId'], $row['SeqId'], $row['SeqNumb'], $row['Level'], $row['AvtorId']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function create_tables($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("
    CREATE TABLE authors(
	id integer primary key,
	letter char(1),
	search_name varchar(255),
	full_name varchar(255),
	first_name varchar(128),
	middle_name varchar(128),
	last_name varchar(128),
	newid integer,
	description text);
  ");

  $sqlite_db->query("
    CREATE TABLE books(
      id integer not null,
      id_author integer not null,
      title varchar(255) not null,
      annotation text,
      genres text,
      id_sequence integer,
      deleted boolean,
      id_archive integer,
      file_name varchar(255),
      file_size integer,
      file_type varchar(20),
      description text);
  ");

  $sqlite_db->query("
    CREATE TABLE archives(
      id integer primary key,
      file_name varchar(255),
      file_path varchar(255),
      file_size integer,
      file_count integer,
      min_id_book integer,
      max_id_book integer,
      file_type varchar(20),
      description text);
  ");

  $sqlite_db->query("CREATE TABLE sequences(id integer primary key, value varchar(255) not null);");

  $sqlite_db->query("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer);");

  $sqlite_db->query("CREATE TABLE params(id integer primary key, value integer, text text);");
  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(text) VALUES ('Test Library');");
  $sqlite_db->query("INSERT INTO params(value) VALUES (1);");

  $sqlite_db->query("CREATE TABLE words(word varchar(99), id_book integer not null, number integer);");

  $sqlite_db->query("commit;");
}

function create_indexes($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("CREATE INDEX author_id ON authors(id);");
  $sqlite_db->query("CREATE INDEX author_letter ON authors(letter);");
  $sqlite_db->query("CREATE INDEX author_name ON authors(search_name);");

  $sqlite_db->query("CREATE INDEX book_id ON books(id);");
  $sqlite_db->query("CREATE INDEX book_author ON books(id_author);");
  $sqlite_db->query("CREATE INDEX book_archive ON books(id_archive);");

  $sqlite_db->query("CREATE INDEX book_file ON archives(file_name);");

  $sqlite_db->query("CREATE INDEX sequences_name ON sequences(value);");

  $sqlite_db->query("CREATE INDEX bookseq_book ON sequences(id_book);");
  $sqlite_db->query("CREATE INDEX bookseq_author ON sequences(id_author);");

  $sqlite_db->query("CREATE INDEX words_word ON words(word);");

  $sqlite_db->query("commit;");
}


$sqlite_db = new PDO('sqlite:./MyRuLib.db');
$mysql_db = new mysqli('localhost', 'root', '', 'lib');
$mysql_db->query("SET NAMES utf8");

create_tables($sqlite_db);
convert_authors($mysql_db, $sqlite_db);
convert_books($mysql_db, $sqlite_db);
convert_seqnames($mysql_db, $sqlite_db);
convert_sequences($mysql_db, $sqlite_db);
create_indexes($sqlite_db);

?>
