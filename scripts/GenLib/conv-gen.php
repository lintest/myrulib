<?php

require_once '../Common/datafile.php';
require_once '../Common/strutils.php';

function InitDatabase($mysql)
{
	$mysql->query("CREATE TABLE auth(aid integer primary key AUTO_INCREMENT, name VARCHAR(300)) DEFAULT CHARACTER SET utf8");
	$mysql->query("CREATE INDEX auth_name ON auth(name)");
}

function GetAuthId($mysql, $name)
{
	if (strlen($name) == 0) return 0; 

	$stmt1 = $mysql->stmt_init();
	$stmt1->prepare("SELECT aid FROM auth WHERE name=?");
	$stmt1->bind_param("s", $param1);
	$param1 = $name;
	$stmt1->execute();
	$stmt1->bind_result($code1);
	if ($stmt1->fetch()) {
		$code = $code1;
		$stmt1->close();
		return $code;
	}
	$stmt1->close();

	$stmt2 = $mysql->stmt_init();
	$stmt2->prepare("INSERT INTO auth(name) VALUES(?)");
	$stmt2->bind_param("s", $param2);
	$param2 = $name;
	$stmt2->execute();
	
	return GetAuthId($mysql, $name);
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

  $auth = 0;
  $name = "";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['Time']." - ".$row['Id']." - ".$row['Extension']." - ".$row['Author']." - ".$row['Title']."\n";
	$next = trim($row['Author']);
	if ($name != $next) {
	  $auth = GetAuthId($mysql_db, $next);
	  $name = $next;
	}
	$file_type = trim($row['Extension']);
	$file_type = trim($file_type, ".");
	$file_type = strtolower($file_type);
	$time = $row['Time'];
	if ($time < 20) $time = 0;

	$descr = NULL;
	$md5 = $row['md5'];
    $sqltest1 = "SELECT descr FROM description WHERE id='$md5'";
    $query1 = $mysql_db->query($sqltest1);
    $row1 = $query1->fetch_array();
	if ($row = $query->fetch_array()) {
	  $descr = $row1['descr'];	
	}
        
	$sql = "INSERT INTO books (id, id_author, title, file_size, file_type, created, lang, md5sum, description) VALUES(?,?,?,?,?,?,?,?,?)";
	$insert = $sqlite_db->prepare($sql);
	$err= $insert->execute(array($row['Id'], $auth, trim($row['Title']), $row['Filesize'], $row['Extension'], $time, $row['Language'], $row['md5'], $descr));
	$insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function convert_auth($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction");

  $sqltest = "SELECT aid, name FROM auth";

  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo $row['aid']." - ".$row['name']."\n";
    $sql = "INSERT INTO authors(id, full_name, last_name) VALUES(?,?,?)";
    $insert = $sqlite_db->prepare($sql);
    $insert->execute(array($row['aid'], $row['name'], $row['name']));
    $insert->closeCursor();
  }
  $query->close(); 
  $sqlite_db->query("commit");
}  

function convert_dates($mysql_db, $sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DELETE FROM dates");

  $sqltest = "
    SELECT DATE_FORMAT(TimeAdded,'%y%m%d') as Time, MAX(id) as Max, MIN(id) as Min, COUNT(id) AS Num
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

function setup_params($sqlite_db, $date, $type)
{
  $sqlite_db->query("begin transaction;");
  
  $sqlite_db->query("DELETE FROM params;");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (1,  'Genesis library');");
  $sqlite_db->query("INSERT INTO params(id,value) VALUES (2,  1);");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (3,  'GENESIS');");
  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (11, 'gen.lib.rus.ec');");
#  $sqlite_db->query("INSERT INTO params(id,value) VALUES (20, $date);");
#  $sqlite_db->query("INSERT INTO params(id,text)  VALUES (21, '$type');");
  
  $sqlite_db->query("commit;");
}

function FullImport($mysql_db, $file, $date)
{
  $sqlite_db = new PDO('sqlite:./'.$file);
  
  create_tables($sqlite_db);
  setup_params($sqlite_db, $date, "FULL");
  
  convert_books($mysql_db, $sqlite_db);
  convert_auth($mysql_db, $sqlite_db);
#  convert_dates($mysql_db, $sqlite_db);
  
  create_indexes($sqlite_db);
}

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '11111111';
$mysql_base = 'bookwarrior';
$sqlitefile = 'myrulib.db';

include('settings.php');

$sqlite_db = new PDO('sqlite:./myrulib.db');
$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

$date = date('Ymd');
echo "Today: ".$date."\n";

InitDatabase($mysql_db);
FullImport($mysql_db, $sqlitefile, $date);
system("zip genesis.db.zip $sqlitefile");
  
?>
