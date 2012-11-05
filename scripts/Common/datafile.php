<?php

function create_tables($sqlite_db)
{
  $sqlite_db->query("begin transaction;");

  $sqlite_db->query("DROP TABLE IF EXISTS authors");
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

  $sqlite_db->query("DROP TABLE IF EXISTS books");
  $sqlite_db->query("
    CREATE TABLE books(
      id integer not null,
      id_author integer,
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
      description text,
      rate text,
      PRIMARY KEY(id,id_author)
      );
  ");
  
  $sqlite_db->query("DROP TABLE IF EXISTS archives");
  $sqlite_db->query("
    CREATE TABLE archives(
      id integer primary key,
      file_name varchar(255),
      file_path varchar(255),
      file_size integer,
      file_count integer);
  ");

  $sqlite_db->query("DROP TABLE IF EXISTS dates");
  $sqlite_db->query("CREATE TABLE dates(id integer primary key, lib_min integer, lib_max integer, lib_num, usr_min integer, usr_max, usr_num integer);");
  
  $sqlite_db->query("DROP TABLE IF EXISTS sequences");
  $sqlite_db->query("CREATE TABLE sequences(id integer primary key, number integer, value varchar(255) not null)");

  $sqlite_db->query("DROP TABLE IF EXISTS bookseq");
  $sqlite_db->query("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer, PRIMARY KEY(id_book,id_seq))");

  $sqlite_db->query("DROP TABLE IF EXISTS genres");
  $sqlite_db->query("CREATE TABLE genres(id_book integer, id_genre CHAR(2), PRIMARY KEY(id_book, id_genre))");
  
  $sqlite_db->query("DROP TABLE IF EXISTS files");
  $sqlite_db->query("CREATE TABLE files(id_book integer, id_archive integer, file_name TEXT, file_path TEXT)");
  
  $sqlite_db->query("DROP TABLE IF EXISTS params");
  $sqlite_db->query("CREATE TABLE params(id integer primary key, value integer, text text)");
  
  $sqlite_db->query("commit;");
}

function create_indexes($sqlite_db)
{
  $sqlite_db->query("begin transaction;");
  
  $sqlite_db->query("INSERT INTO authors(id, letter, full_name) VALUES(0,'#','(empty)')");

  $sqlite_db->query("CREATE INDEX author_letter ON authors(letter);");
  $sqlite_db->query("CREATE INDEX author_name ON authors(search_name);");

  $sqlite_db->query("CREATE INDEX book_author ON books(id_author);");
  $sqlite_db->query("CREATE INDEX book_archive ON books(id_archive);");
  $sqlite_db->query("CREATE INDEX book_md5sum ON books(md5sum);");
  $sqlite_db->query("CREATE INDEX book_created ON books(created);");

  $sqlite_db->query("CREATE INDEX files_book ON files(id_book)");

  $sqlite_db->query("CREATE INDEX sequences_name ON sequences(value);");

  $sqlite_db->query("CREATE INDEX bookseq_seq ON bookseq(id_seq);");
  $sqlite_db->query("CREATE INDEX genres_genre ON genres(id_genre);");

  $sqlite_db->query("commit;");

  $sqlite_db->query("vacuum;");
}

?>
