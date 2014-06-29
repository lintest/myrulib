<?php

require_once 'conv_info.php';

$mysql_srvr = 'localhost';
$mysql_user = 'root';
$mysql_pass = '11111111';
$mysql_base = 'flibusta';
$sqlitefile = 'myrulib.db';

include('settings.php');

$sqlite_db = new PDO('sqlite:'.$sqlitefile);
$mysql_db = new mysqli($mysql_srvr, $mysql_user, $mysql_pass, $mysql_base);
$mysql_db->query("SET NAMES utf8");

author_info($mysql_db, $sqlite_db, 0);
book_info($mysql_db, $sqlite_db, 0);
system("zip flibusta.db.full.zip $sqlitefile");

?>
