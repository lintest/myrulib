<?php

require_once 'bbcode/bbcode.lib.php';

function author_info($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $bb = new bbcode;
  $bb->autolinks = false;
  $sqltest = "SELECT AvtorId FROM libaannotations WHERE AvtorId>$min";
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

function book_info($mysql_db, $sqlite_db, $min)
{
  $sqlite_db->query("begin transaction;");

  $bb = new bbcode;
  $bb->autolinks = false;
  $sqltest = "SELECT BookId FROM libbannotations WHERE BookId>$min";
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

?>
