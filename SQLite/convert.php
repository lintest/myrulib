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

function convert_authors($mysql_db, $sqlite_db)
{
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

    $sql = "INSERT INTO authors (id, letter, full_name, search_name) VALUES(?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['AvtorId'], $letter, $full_name, $search_name));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
}

function convert_books($mysql_db, $sqlite_db)
{
  $sqlite_db->query("DELETE FROM books");

  $sqltest = "
    SELECT libbook.BookId, FileSize, Title, Deleted, CASE WHEN AvtorId IS NULL THEN 0 ELSE AvtorId END AS AvtorId
    FROM libbook LEFT JOIN libavtor ON libbook.BookId = libavtor.BookId 
    WHERE Deleted<>1
  ";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['BookId']." - ".$row['AvtorId']." - ".$row['Title']."\n";
    $filename = $row['BookId'].".".$row['FileType'];
    $sql = "INSERT INTO books (id, id_author, title, deleted, file_name, file_size, file_type) VALUES(?,?,?,?,?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    if($insert === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $err= $insert->execute(array($row['BookId'], $row['AvtorId'], $row['Title'], $row['Deleted'], $filename, $row['FileSize'], $row['FileType']));
    if($err === false){ $err= $dbh->errorInfo(); die($err[2]); }
    $insert->closeCursor();
  }
}

function fix_avtoraliase($mysql_db, $sqlite_db)
{
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
}

function convert_seqnames($mysql_db, $sqlite_db)
{
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
}

function convert_sequences($mysql_db, $sqlite_db)
{
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
}

//"CREATE TABLE sequences(id integer primary key, value varchar(255) not null);"));


$sqlite_db = new PDO('sqlite:/home/user/projects/MyRuLib/build/Release/MyRuLib.db');
$mysql_db = new mysqli('localhost', 'root', '', 'lib');
$mysql_db->query("SET NAMES utf8");

convert_authors($mysql_db, $sqlite_db);
convert_books($mysql_db, $sqlite_db);
convert_seqnames($mysql_db, $sqlite_db);
convert_sequences($mysql_db, $sqlite_db);
//fix_avtoraliase($mysql_db, $sqlite_db);

?>