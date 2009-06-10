#include "FbGenres.h"

struct GenreStruct {
	wxChar letter;
	wxString code;
	wxString name;
};

const GenreStruct genres_list[] = {
    {wxT('0'), wxT("sf_history"), _("Альтернативная история")},
    {wxT('1'), wxT("sf_action"), _("Боевая фантастика")},
    {wxT('2'), wxT("sf_epic"), _("Эпическая фантастика")},
    {wxT('3'), wxT("sf_heroic"), _("Героическая фантастика")},
    {wxT('4'), wxT("sf_detective"), _("Детективная фантастика")},
    {wxT('5'), wxT("sf_cyberpunk"), _("Киберпанк")},
    {wxT('6'), wxT("sf_space"), _("Космическая фантастика")},
    {wxT('7'), wxT("sf_social"), _("Социально-психологическая фантастика")},
    {wxT('8'), wxT("sf_horror"), _("Ужасы и Мистика")},
    {wxT('9'), wxT("sf_humor"), _("Юмористическая фантастика")},
    {wxT('A'), wxT("sf_fantasy"), _("Фэнтези")},
    {wxT('B'), wxT("sf"), _("Научная Фантастика")},
    {wxT('C'), wxT("det_classic"), _("Классический детектив")},
    {wxT('D'), wxT("det_police"), _("Полицейский детектив")},
    {wxT('E'), wxT("det_action"), _("Боевик")},
    {wxT('F'), wxT("det_irony"), _("Иронический детектив")},
    {wxT('G'), wxT("det_history"), _("Исторический детектив")},
    {wxT('H'), wxT("det_espionage"), _("Шпионский детектив")},
    {wxT('I'), wxT("det_crime"), _("Криминальный детектив")},
    {wxT('J'), wxT("det_political"), _("Политический детектив")},
    {wxT('K'), wxT("det_maniac"), _("Маньяки")},
    {wxT('L'), wxT("det_hard"), _("Крутой детектив")},
    {wxT('M'), wxT("thriller"), _("Триллер")},
    {wxT('N'), wxT("detective"), _("Детектив")},
    {wxT('O'), wxT("prose_classic"), _("Классическая проза")},
    {wxT('P'), wxT("prose_history"), _("Историческая проза")},
    {wxT('Q'), wxT("prose_contemporary"), _("Современная проза")},
    {wxT('R'), wxT("prose_counter"), _("Контркультура")},
    {wxT('S'), wxT("prose_rus_classic"), _("Русская классическая проза")},
    {wxT('T'), wxT("prose_su_classics"), _("Советская классическая проза")},
    {wxT('U'), wxT("love_contemporary"), _("Современные любовные романы")},
    {wxT('V'), wxT("love_history"), _("Исторические любовные романы")},
    {wxT('W'), wxT("love_detective"), _("Остросюжетные любовные романы")},
    {wxT('X'), wxT("love_short"), _("Короткие любовные романы")},
    {wxT('Y'), wxT("love_erotica"), _("Эротика")},
    {wxT('Z'), wxT("adv_western"), _("Вестерн")},
    {wxT('a'), wxT("adv_history"), _("Исторические приключения")},
    {wxT('b'), wxT("adv_indian"), _("Приключения про индейцев")},
    {wxT('c'), wxT("adv_maritime"), _("Морские приключения")},
    {wxT('d'), wxT("adv_geo"), _("Путешествия и география")},
    {wxT('e'), wxT("adv_animal"), _("Природа и животные")},
    {wxT('f'), wxT("adventure"), _("Приключения")},
    {wxT('g'), wxT("child_tale"), _("Сказка")},
    {wxT('h'), wxT("child_verse"), _("Детские стихи")},
    {wxT('i'), wxT("child_prose"), _("Детскиая проза")},
    {wxT('j'), wxT("child_sf"), _("Детская фантастика")},
    {wxT('k'), wxT("child_det"), _("Детские остросюжетные")},
    {wxT('l'), wxT("child_adv"), _("Детские приключения")},
    {wxT('m'), wxT("child_education"), _("Детская образовательная литература")},
    {wxT('n'), wxT("children"), _("Детская литература")},
    {wxT('o'), wxT("poetry"), _("Поэзия")},
    {wxT('p'), wxT("dramaturgy"), _("Драматургия")},
    {wxT('q'), wxT("antique_ant"), _("Античная литература")},
    {wxT('r'), wxT("antique_european"), _("Европейская старинная литература")},
    {wxT('s'), wxT("antique_russian"), _("Древнерусская литература")},
    {wxT('t'), wxT("antique_east"), _("Древневосточная литература")},
    {wxT('u'), wxT("antique_myths"), _("Мифы. Легенды. Эпос")},
    {wxT('v'), wxT("antique"), _("Старинная литература")},
    {wxT('w'), wxT("sci_history"), _("История")},
    {wxT('x'), wxT("sci_psychology"), _("Психология")},
    {wxT('y'), wxT("sci_culture"), _("Культурология")},
    {wxT('z'), wxT("sci_religion"), _("Религиоведение")},
    {wxT('А'), wxT("sci_philosophy"), _("Философия")},
    {wxT('Б'), wxT("sci_politics"), _("Политика")},
    {wxT('В'), wxT("sci_business"), _("Деловая литература")},
    {wxT('Г'), wxT("sci_juris"), _("Юриспруденция")},
    {wxT('Д'), wxT("sci_linguistic"), _("Языкознание")},
    {wxT('Е'), wxT("sci_medicine"), _("Медицина")},
    {wxT('Ё'), wxT("sci_phys"), _("Физика")},
    {wxT('Ж'), wxT("sci_math"), _("Математика")},
    {wxT('З'), wxT("sci_chem"), _("Химия")},
    {wxT('И'), wxT("sci_biology"), _("Биология")},
    {wxT('Й'), wxT("sci_tech"), _("Технические науки")},
    {wxT('К'), wxT("science"), _("Научная литература")},
    {wxT('Л'), wxT("comp_www"), _("Интернет")},
    {wxT('М'), wxT("comp_programming"), _("Программирование")},
    {wxT('Н'), wxT("comp_hard"), _("Компьютерное железо (аппаратное обеспечение)")},
    {wxT('О'), wxT("comp_soft"), _("Программы")},
    {wxT('П'), wxT("comp_db"), _("Базы данных")},
    {wxT('Р'), wxT("comp_osnet"), _("ОС и Сети")},
    {wxT('С'), wxT("computers"), _("Компьтерная литература")},
    {wxT('Т'), wxT("ref_encyc"), _("Энциклопедии")},
    {wxT('У'), wxT("ref_dict"), _("Словари")},
    {wxT('Ф'), wxT("ref_ref"), _("Справочники")},
    {wxT('Х'), wxT("ref_guide"), _("Руководства")},
    {wxT('Ц'), wxT("reference"), _("Справочная литература")},
    {wxT('Ч'), wxT("nonf_biography"), _("Биографии и Мемуары")},
    {wxT('Ш'), wxT("nonf_publicism"), _("Публицистика")},
    {wxT('Щ'), wxT("nonf_criticism"), _("Критика")},
    {wxT('Ъ'), wxT("design"), _("Искусство и Дизайн")},
    {wxT('Ы'), wxT("nonfiction"), _("Документальная литература")},
    {wxT('Ь'), wxT("religion_rel"), _("Религия")},
    {wxT('Э'), wxT("religion_esoterics"), _("Эзотерика")},
    {wxT('Ю'), wxT("religion_self"), _("Самосовершенствование")},
    {wxT('Я'), wxT("religion"), _("Религионая литература")},
    {wxT('а'), wxT("humor_anecdote"), _("Анекдоты")},
    {wxT('б'), wxT("humor_prose"), _("Юмористическая проза")},
    {wxT('в'), wxT("humor_verse"), _("Юмористические стихи")},
    {wxT('г'), wxT("humor"), _("Юмор")},
    {wxT('д'), wxT("home_cooking"), _("Кулинария")},
    {wxT('е'), wxT("home_pets"), _("Домашние животные")},
    {wxT('ё'), wxT("home_crafts"), _("Хобби и ремесла")},
    {wxT('ж'), wxT("home_entertain"), _("Развлечения")},
    {wxT('з'), wxT("home_health"), _("Здоровье")},
    {wxT('и'), wxT("home_garden"), _("Сад и огород")},
    {wxT('й'), wxT("home_diy"), _("Сделай сам")},
    {wxT('к'), wxT("home_sport"), _("Спорт")},
    {wxT('л'), wxT("home_sex"), _("Эротика, Секс")},
    {wxT('м'), wxT("home"), _("Домоводство")},
    {0, wxEmptyString, wxEmptyString},
};

const size_t genres_count = sizeof(genres_list) / sizeof(GenreStruct);

wxString FbGenres::Char(wxString &code)
{
    for (size_t i=0; genres_list[i].letter; i++)
        if (genres_list[i].code == code)
            return genres_list[i].letter;
    return wxEmptyString;
}

wxString FbGenres::Code(wxChar &letter)
{
    for (size_t i=0; genres_list[i].letter; i++)
        if (genres_list[i].letter == letter)
            return genres_list[i].code;
    return wxEmptyString;
}

wxString FbGenres::Name(wxChar &letter)
{
    for (size_t i=0; genres_list[i].letter; i++)
        if (genres_list[i].letter == letter)
            return genres_list[i].name;
    return wxEmptyString;
}

