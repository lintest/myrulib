<?php

function GenreCode($code)
{
 global $genres;
  if(!isset($genres)){
    $genres = array(
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
      "prose_military" => "37",
      "prose" => "30",
      "love_contemporary" => "41",
      "love_history" => "42",
      "love_detective" => "43",
      "love_short" => "44",
      "love_erotica" => "45",
      "love" => "40",
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
      "geo_guides" => "FA",
      "job_hunting" => "01",
      "management" => "02",
      "marketing" => "03",
      "banking" => "04",
      "stock" => "05",
      "accounting" => "06",
      "global_economy" => "07",
      "economics" => "00",
      "industries" => "08",
      "org_behavior" => "09",
      "personal_finance" => "0A",
      "real_estate" => "0B",
      "popular_business" => "0C",
      "small_business" => "0D",
      "paper_work" => "0E",
      "economics_ref" => "0F",
    );
  }
  if (array_key_exists($code, $genres)) return $genres[$code];
  return "";
}

function GetGenre($mysql, $code, $name)
{
	$stmt1 = $mysql->stmt_init();
	$stmt1->prepare("SELECT gid FROM myrulib_genres WHERE code=?");
	$stmt1->bind_param("s", $param1);
	$param1 = $code;
	$stmt1->execute();
	$stmt1->bind_result($code1);
	if ($stmt1->fetch()) {
		$gid = $code1;
		$stmt1->close();
		return $gid;
	}
	$stmt1->close();

	$stmt2 = $mysql->stmt_init();
	$stmt2->prepare("INSERT INTO myrulib_genres(code, name) VALUES(?,?)");
	$stmt2->bind_param("ss", $param21, $param22);
	$param21 = $code;
	$param22 = $name;
	$stmt2->execute();

	$stmt3 = $mysql->stmt_init();
	$stmt3->prepare("SELECT gid FROM myrulib_genres WHERE code=?");
	$stmt3->bind_param("s", $param3);
	$param3 = $code;
	$stmt3->execute();
	$stmt3->bind_result($code3);
	if ($stmt1->fetch()) {
		$gid = $code3;
		$stmt3->close();
		return $gid;
	}
	$stmt3->close();

	return 0;
}

function KeyToChar($key)
{

}

function ConvertGenre($mysql_db, $code, $name)
{
 $code = trim($code);
 if (strlen($code) == 0) return ""; 
 $code = strtolower($code);

 global $genres;
  if(!isset($genres)){
    $genres = array(
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
      "prose_military" => "37",
      "prose" => "30",
      "love_contemporary" => "41",
      "love_history" => "42",
      "love_detective" => "43",
      "love_short" => "44",
      "love_erotica" => "45",
      "love" => "40",
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
      "geo_guides" => "FA",
      "job_hunting" => "01",
      "management" => "02",
      "marketing" => "03",
      "banking" => "04",
      "stock" => "05",
      "accounting" => "06",
      "global_economy" => "07",
      "economics" => "00",
      "industries" => "08",
      "org_behavior" => "09",
      "personal_finance" => "0A",
      "real_estate" => "0B",
      "popular_business" => "0C",
      "small_business" => "0D",
      "paper_work" => "0E",
      "economics_ref" => "0F",
    );
  }
  if (array_key_exists($code, $genres)) return $genres[$s];

  $gid = GetGenre($mysql_db, $code, $name);

  if ($gid == 0) return "";

  $genres[$code] = $key;

  return "";
}

function InitGenres($mysql_db)
{
  $mysql_db->query("CREATE TABLE IF NOT EXISTS myrulib_genres(gid INTEGER NOT NULL AUTO_INCREMENT, code VARCHAR(50), name VARCHAR(200))");
}

?>
