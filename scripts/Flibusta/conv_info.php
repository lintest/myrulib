<?php

require_once 'bbcode/bbcode.lib.php';

function author_info($mysql_db, $sqlite_db)
{
  

  $bb = new bbcode;
  $bb->autolinks = false;
  $sqltest = "SELECT AvtorId FROM libaannotations";
  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo "Auth: ".$row['AvtorId']."\n";

    $sqltest1 = "SELECT Body FROM libaannotations WHERE AvtorId=".$row['AvtorId'];
    $query1 = $mysql_db->query($sqltest1);
    $row1 = $query1->fetch_array();
        
    $sql = "UPDATE authors SET description=? where id=?";
    $insert = $sqlite_db->prepare($sql);
    $bb -> parse($row1['Body']);
    $body = $bb->get_html();
    $body = str_replace("&lt;", "<", $body);
    $body = str_replace("&gt;", ">", $body);
    $insert->execute(array($body, $row['AvtorId']));
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

function book_info($mysql_db, $sqlite_db)
{
  $bb = new bbcode;
  $bb->autolinks = false;
  $sqltest = "SELECT BookId FROM libbannotations";
  $query = $mysql_db->query($sqltest);
  while ($row = $query->fetch_array()) {
    echo "Book: ".$row['BookId']."\n";
    
    $sqltest1 = "SELECT Body FROM libbannotations WHERE BookId=".$row['BookId'];
    $query1 = $mysql_db->query($sqltest1);
    $row1 = $query1->fetch_array();
        
    $sql = "UPDATE books SET description=? where id=?";
    $insert = $sqlite_db->prepare($sql);
    $bb -> parse($row1['Body']);
	$body = $bb->get_html();
    $body = str_replace("&lt;", "<", $body);
    $body = str_replace("&gt;", ">", $body);
    $insert->execute(array($body, $row['BookId']));
    $insert->closeCursor();
  }

  $sqlite_db->query("commit;");
}

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '';
$mysql_base = 'flibusta';
$sqlitefile = './myrulib.db';

include('settings.php');

$sqlite_db = new PDO('sqlite:'.$sqlitefile);
$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

author_info($mysql_db, $sqlite_db);
book_info($mysql_db, $sqlite_db);

?>
