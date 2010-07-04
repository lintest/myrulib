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

function convert_books($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM books");
  $sqlite_db->query("DELETE FROM authors");

  $mysql_db->real_query("DELETE FROM authors");

  $sqltest = "
	SELECT Id, Title, Author, Language, Topic, Filesize, Extension, DATE_FORMAT(TimeAdded,'%y%m%d') As Time, identifier, md5, coverurl
	FROM updated
	ORDER BY Id 
  ";

  $new_author = 1;

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['Time']." - ".$row['Id']." - ".$row['Extension']." - ".$row['Author']." - ".$row['Title']."\n";

	$list = explode(',', trim($row['Author']));
	$count = count($list);
    for($i = 0; $i < $count; $i++) {
	  $auth_name = trim($list[$i]);
	  $search = $sqlite_db->prepare("SELECT id FROM authors WHERE full_name=?");
	  $search->execute(array($auth_name));
	  $red = $search->fetchAll();
	  $auth_id = 0;
	  foreach ($red as $authrow) {
		$auth_id = $authrow['id'];
	  }
	  if ($auth_id == 0) {
		$auth_id = $new_author;
		echo $auth_id." - ".$auth_name."\n";
		$insert = $sqlite_db->prepare("INSERT INTO authors(id, full_name, last_name) VALUES(?,?,?)");
		$insert->execute(array($auth_id, $auth_name, $auth_name));
		$insert->execute();
		$insert->closeCursor();
		$new_author = $new_author + 1;
	  }
	  $file_type = trim($row['Extension']);
	  $file_type = trim($file_type, ".");
	  $file_type = strtolower($file_type);
	  $file_type = strtolowerEx($file_type);
	  $time = $row['Time'];
	  if ($time > 20) $time = 0;
	  $sql = "INSERT INTO books (id, id_author, title, file_size, file_type, created, lang, md5sum) VALUES(?,?,?,?,?,?,?,?)";
	  $insert = $sqlite_db->prepare($sql);
	  $err= $insert->execute(array($row['Id'], $auth_id, trim($row['Title']), $row['Filesize'], $row['Extension'], $time, $row['Language'], $row['md5']));
	  $insert->closeCursor();
	}
  }

  $sqlite_db->query("commit;");
}

function convert_dates($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM dates");

  $sqltest = "
    SELECT DATE_FORMAT(TimeAdded,'%y%m%d') as Time, MAX(bid) as Max, MIN(bid) as Min, COUNT(bid) AS Num
	FROM updated
	GROUP BY DATE_FORMAT(TimeLastModified,'%y%m%d') 
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
	echo $row['Time']." - ".$row['Max']." - ".$row['Min']."\n";
    $sql = "INSERT INTO dates (id, lib_max, lib_min, lib_num) VALUES(?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['Time'], $row['Max'], $row['Min'], $row['Num']));
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
	number integer,
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
      file_name text,
      file_size integer,
      file_type varchar(20),
      md5sum char(32),
      created integer,
      lang char(2),
      year integer,
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

  $sqlite_db->query("CREATE TABLE sequences(id integer primary key, number integer, value varchar(255) not null);");

  $sqlite_db->query("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer);");

  $sqlite_db->query("CREATE TABLE params(id integer primary key, value integer, text text);");
  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(text) VALUES ('LibRusEc Library');");
  $sqlite_db->query("INSERT INTO params(value) VALUES (1);");
  $sqlite_db->query("INSERT INTO params(text) VALUES ('LIBRUSEC');");
  $sqlite_db->query("INSERT INTO params(id,text) VALUES (11,'lib.rus.ec');");

  $sqlite_db->query("CREATE TABLE aliases(id_author integer not null, id_alias integer not null);");
  
  $sqlite_db->query("CREATE TABLE genres(id_book integer, id_genre CHAR(2));");
  
  $sqlite_db->query("CREATE TABLE dates(id integer primary key, lib_min integer, lib_max integer, lib_num, usr_min integer, usr_max, usr_num integer);");

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
  $sqlite_db->query("CREATE INDEX book_md5sum ON books(md5sum);");
  $sqlite_db->query("CREATE INDEX book_created ON books(created);");
  
  $sqlite_db->query("CREATE INDEX book_file ON archives(file_name);");

  $sqlite_db->query("CREATE INDEX sequences_name ON sequences(value);");

  $sqlite_db->query("CREATE INDEX bookseq_book ON bookseq(id_book);");
  $sqlite_db->query("CREATE INDEX bookseq_author ON bookseq(id_author);");

  $sqlite_db->query("CREATE INDEX aliases_author ON aliases(id_author);");
  $sqlite_db->query("CREATE INDEX aliases_alias ON aliases(id_alias);");
  
  $sqlite_db->query("CREATE INDEX genres_book ON genres(id_book);");
  $sqlite_db->query("CREATE INDEX genres_genre ON genres(id_genre);");

  $sqlite_db->query("commit;");
}

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '';
$mysql_base = 'bookwarrior';

include('settings.php');

$sqlite_db = new PDO('sqlite:./myrulib.db');

$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

create_tables($sqlite_db);
convert_books($mysql_db, $sqlite_db);

#convert_genres($mysql_db, $sqlite_db);
#convert_authors($mysql_db, $sqlite_db);
#convert_dates($mysql_db, $sqlite_db);
#convert_seqnames($mysql_db, $sqlite_db);
#convert_sequences($mysql_db, $sqlite_db);
#create_indexes($sqlite_db);

?>
