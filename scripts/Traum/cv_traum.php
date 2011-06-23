<?php

require_once '../Common/datafile.php';
require_once '../Common/genres.php';
require_once '../Common/strutils.php';

function utf($string)
{
  return iconv("WINDOWS-1251", "UTF-8", $string); 
}

function letter($string)
{
	$char_list = 'А Б В Г Д Е Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ы Э Ю Я A B C D E F G H I J K L M N O P Q R S T U V W X Y Z';
	$letter = utf8_substr($string,0,1);
	$letter = strtoupperEx($letter,0,1);
	if (strpos($char_list, $letter) === false) { $letter = "#"; };
	return $letter; 
}

function convert_auth($sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM authors");

  $handle = fopen("db/author", "r");
  while (!feof($handle)) {
	$buffer = Trim(fgets($handle, 4096),"\n\r");
	if(empty($buffer{0})) continue;
	$fields = explode(chr(9), $buffer);
	$id = $fields[0];
	$names = explode(",", $fields[1].","); 
	$last_name = utf(trim($names[0]));
	$first_name = utf(trim($names[1]));
	$middle_name = utf(trim($fields[2]));
	$full_name = trim(trim($last_name." ".$first_name)." ".$middle_name);
	$letter = letter($full_name);
	$numb = $fields[5];
	echo "Auth: ".$letter." - ".$id." - ".$full_name."\n";
	$sql = "INSERT INTO authors (id, number, letter, full_name, first_name, middle_name, last_name) VALUES(?,?,?,?,?,?,?)";
	$insert = $sqlite_db->prepare($sql);
	$insert->execute(array($id, $numb, $letter, $full_name, $first_name, $middle_name, $last_name));
  }
  fclose($handle);
  $sqlite_db->query("commit;");
}  

function convert_seqn($sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");
  $sqlite_db->query("DELETE FROM sequences");
  $sqlite_db->query("CREATE TEMP TABLE temp_seqn(id integer primary key, value text)");

  $handle = fopen("db/series", "r");
  while (!feof($handle)) {
	$buffer = Trim(fgets($handle, 4096),"\n\r");
	if(empty($buffer{0})) continue;
	$fields = explode(chr(9), $buffer);
	$code = $fields[0];
	$name = utf(trim($fields[1]));
	echo "Seqn: ".$code." - ".$name."\n";
	$sql = "INSERT INTO temp_seqn(id, value) VALUES(?,?)";
	$insert = $sqlite_db->prepare($sql);
	$insert->execute(array($code, $name));
  }
  fclose($handle);

  $sql = "INSERT INTO sequences(id, value, number) SELECT id, value, COUNT(DISTINCT id_book) FROM temp_seqn LEFT JOIN bookseq ON id_seq=id GROUP BY id, value";
  $sqlite_db->query($sql);
  $sqlite_db->query("commit;");
}

function convert_book($sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");
  $sqlite_db->query("DELETE FROM books");

  $handle = fopen("db/book", "r");
  while (!feof($handle)) {
	$buffer = Trim(fgets($handle, 4096),"\n\r");
	if(empty($buffer{0})) continue;
	$fields = explode(chr(9), $buffer);
	$book  = $fields[0];
	$auth  = $fields[1];
	$title = utf(trim($fields[2]));
	$lang  = utf(trim($fields[3]));
	$seqn  = $fields[4];
	$numb  = $fields[5];
	$year  = $fields[6];
	$arch  = utf(trim(str_replace("\\","/",$fields[8]))).utf(trim($fields[9])).".zip";
	$file  = utf(trim($fields[9]));
	$type  = substr(strrchr($file, '.'), 1);
	$size  = $fields[10];
	$arsz  = $fields[11];
	$date  = $fields[12];
	$crc32 = $fields[13]; 
	if ($date > 2000000) $date = $date - 20000000; 
	
	echo "Book: ".$book." - ".$type." - ".$title."\n";

	$sql = "INSERT INTO books(id, id_archive, id_author, title, file_name, file_size, file_type, genres, created, lang, year, md5sum) VALUES(?,?,?,?,?,?,?,?,?,?,?,?)";
	$insert = $sqlite_db->prepare($sql);
	$insert->execute(array($book, $book, $auth, $title, $file, $size, $type, NULL, $date, $lang, $year, $crc32));

	$sql = "INSERT INTO archives(id, file_name, file_size, file_count) VALUES(?,?,?,1)";
	$insert = $sqlite_db->prepare($sql);
	$insert->execute(array($book, $arch, $arsz));

	if ($seqn != 0) {
		$sql = "INSERT INTO bookseq(id_book, id_seq, number) VALUES(?,?,?)";
		$insert = $sqlite_db->prepare($sql);
		$insert->execute(array($book, $seqn, $numb));
	}
  }
  fclose($handle);
}  

function convert_genr($sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");
  $sqlite_db->query("DELETE FROM genres");

  $prior = 0;
  $fields = "";
  $genres = "";
  $handle = fopen("db/booktags", "r");
  while (true) {
	if (feof($handle)) {
		$book = 0;
	} else {
		$buffer = Trim(fgets($handle, 4096),"\n\r");
		if(empty($buffer{0})) continue;
		$fields = explode(chr(9), $buffer);
		$book = $fields[0];
		$code = Trim($fields[1]);
		$code = Trim($code,"\n\r");
		$char = GenreCode($code);
	}
	if ($prior && $book != $prior) {
		echo "Genr: ".$prior." - ".$genres."\n";
		$sql = "UPDATE books SET genres=? WHERE id=?";
		$insert = $sqlite_db->prepare($sql);
		$insert->execute(array($genres, $prior));
		$genres = "";
	}
	if (feof($handle)) break;
	if (!empty($char{0})) {
		echo "Genr: ".$book." - ".$char." - ".$code."\n";
		$sql = "INSERT INTO genres(id_book, id_genre) VALUES(?,?)";
		$insert = $sqlite_db->prepare($sql);
		$insert->execute(array($book, $char));
		$genres = $genres.$char;
	}
	$prior = $book;
  }
  fclose($handle);
  $sqlite_db->query("commit;");
}

function convert_info($sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $text = "";
  $prior = 0;
  $handle = fopen("db/bookanno", "r");
  while (true) {
	if (feof($handle)) {
		$book = -1;
	} else {
		$buffer = Trim(fgets($handle, 4096),"\n\r");
		if(empty($buffer{0})) continue;
		$fields = explode(chr(9), $buffer);
		$book = $fields[0];
	}
	if ($prior && $book != $prior) {
		while (substr($text, 0, 2)=="\\n") $text=substr($text, 2);
		while (substr($text, -2)=="\\n") $text=substr($text, 0, -2);
		$text = str_replace("\\n\\n", "\n", $text);
		$text = str_replace("\\n", "</p><p>", $text);
		$text = "<p>".$text."</p>";
		echo "Info: ".$prior." - ".substr($text, 0, 50)."\n";
		$sql = "UPDATE books SET description=? where id=?";
		$insert = $sqlite_db->prepare($sql);
		$insert->execute(array($text, $prior));
		$text = utf(trim($fields[1]));
	} else if ($book) {
		$text = $text."\\n".utf(trim($fields[1]));
	}
	if (feof($handle)) break;
	$prior = $book;
  }
  fclose($handle);
  $sqlite_db->query("commit;");
}

function convert_date($sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");
  $sqlite_db->query("DELETE FROM dates");
  $sql = "INSERT INTO dates (id, lib_min, lib_max, lib_num) SELECT created, MIN(id), MAX(id), COUNT(DISTINCT id) FROM books GROUP BY created";
  $insert = $sqlite_db->query($sql);
  $sqlite_db->query("commit;");
}  

function setup_params($sqlite_db, $date, $type)
{
  $sqlite_db->query("begin transaction;");
  
  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (1,  'Traum library');");
  $sqlite_db->query("INSERT INTO params(id,value) VALUES (2,  1);");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (3,  'TRAUM');");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (11, 'traumlibrary.net');");
  $sqlite_db->query("INSERT INTO params(id,value) VALUES (20, $date);");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (21, '$type');");
  
  $sqlite_db->query("commit;");
}

function FullImport($file, $date)
{
  $sqlite_db = new PDO('sqlite:./'.$file);
  
  create_tables($sqlite_db);
  setup_params($sqlite_db, $date, "FULL");
  convert_auth($sqlite_db, 0);
  convert_book($sqlite_db, 0);
  convert_seqn($sqlite_db, 0);
  convert_date($sqlite_db, 0);
  convert_genr($sqlite_db, 0);
  convert_info($sqlite_db, 0);
  create_indexes($sqlite_db);
/*
  convert_sequences($mysql_db, $sqlite_db, 0);
  convert_dates($mysql_db, $sqlite_db, 0);
  
  convert_zips($mysql_db, $sqlite_db, 0);
  convert_files($mysql_db, $sqlite_db, 0, 0);
*/
}

$sqlitefile = 'myrulib.db';

$date = date('Ymd');
echo "Today: ".$date."\n";
system("rm $sqlitefile");
$sqlite_db = new PDO('sqlite:'.$sqlitefile);
FullImport($sqlitefile, $date);
system("zip traumlib.db.zip $sqlitefile");

?>
