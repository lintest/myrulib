<?php

function CreateTables($sqlite_db)
{
  $sqlite_db->query("begin transaction");

  $sqlite_db->query("
	CREATE TABLE a(
	  Aid integer primary key,
	  First text,
	  Middle text,
	  Last text,
	  Full varchar(255),
	  Find varchar(255),
	  Letter char(1),
	  Numb integer,
	  Dscr text);
  ");

  $sqlite_db->query("
	CREATE TABLE b(
	  Bid integer primary key,
	  Zid integer,
	  Aid integer,
	  Title varchar(255) not null,
	  File text,
	  Path text,
	  Size integer,
	  Type text,
	  Md5s text,
	  Genr text,
	  Lang text,
	  Date integer,
	  Year integer,
	  Dscr text);
  ");
  
  $sqlite_db->query("
	CREATE TABLE s(
	  Sid integer primary key,
	  Seqn text,
	  Numb integer);
  ");

  $sqlite_db->query("
	CREATE TABLE ba(
	  Bid integer not null,
	  Aid integer);
  ");
  
  $sqlite_db->query("
	CREATE TABLE bg(
	  Bid integer not null,
	  Gid text);
  ");

  $sqlite_db->query("
	CREATE TABLE bs(
	  Bid integer not null,
	  Sid integer,
	  Num integer);
  ");
  
  $sqlite_db->query("CREATE TABLE params(id integer primary key, value text)");
  
  $sqlite_db->query("commit");
}

?>
