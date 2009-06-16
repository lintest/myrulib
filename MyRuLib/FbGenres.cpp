#include "FbGenres.h"

struct GenreStruct {
	char hi;
	char lo;
	wxChar letter;
	wxString code;
	wxString name;
};

struct HiGenreStruct {
	char hi;
	wxString name;
};

const HiGenreStruct hi_genres[] = {
	{ 0x1, _("Фантастика")},
	{ 0x2, _("Детективы и Триллеры")},
	{ 0x3, _("Проза")},
	{ 0x4, _("Любовные романы")},
	{ 0x5, _("Приключения")},
	{ 0x6, _("Детская литература")},
	{ 0x7, _("Поэзия, Драматургия")},
	{ 0x8, _("Старинное")},
	{ 0x9, _("Наука, Образование")},
	{ 0x9, _("Компьютеры и Интернет")},
	{ 0x9, _("Справочная литература")},
	{ 0x9, _("Документальная литература")},
	{ 0x9, _("Религия и духовность")},
	{ 0x9, _("Юмор")},
	{ 0x9, _("Домоводство (Дом и семья)")},
	{ 0, wxEmptyString},
};

const GenreStruct genres_list[] = {
    { 0x1, 0x1, wxT('0'), wxT("sf_history"), _("Альтернативная история")},
    { 0x1, 0x2, wxT('1'), wxT("sf_action"), _("Боевая фантастика")},
    { 0x1, 0x3, wxT('2'), wxT("sf_epic"), _("Эпическая фантастика")},
    { 0x1, 0x4, wxT('3'), wxT("sf_heroic"), _("Героическая фантастика")},
    { 0x1, 0x5, wxT('4'), wxT("sf_detective"), _("Детективная фантастика")},
    { 0x1, 0x6, wxT('5'), wxT("sf_cyberpunk"), _("Киберпанк")},
    { 0x1, 0x7, wxT('6'), wxT("sf_space"), _("Космическая фантастика")},
    { 0x1, 0x8, wxT('7'), wxT("sf_social"), _("Социально-психологическая фантастика")},
    { 0x1, 0x9, wxT('8'), wxT("sf_horror"), _("Ужасы и Мистика")},
    { 0x1, 0xA, wxT('9'), wxT("sf_humor"), _("Юмористическая фантастика")},
    { 0x1, 0xB, wxT('A'), wxT("sf_fantasy"), _("Фэнтези")},
    { 0x1, 0x0, wxT('B'), wxT("sf"), _("Научная Фантастика")},
    { 0x2, 0x1, wxT('C'), wxT("det_classic"), _("Классический детектив")},
    { 0x2, 0x2, wxT('D'), wxT("det_police"), _("Полицейский детектив")},
    { 0x2, 0x3, wxT('E'), wxT("det_action"), _("Боевик")},
    { 0x2, 0x4, wxT('F'), wxT("det_irony"), _("Иронический детектив")},
    { 0x2, 0x5, wxT('G'), wxT("det_history"), _("Исторический детектив")},
    { 0x2, 0x6, wxT('H'), wxT("det_espionage"), _("Шпионский детектив")},
    { 0x2, 0x7, wxT('I'), wxT("det_crime"), _("Криминальный детектив")},
    { 0x2, 0x8, wxT('J'), wxT("det_political"), _("Политический детектив")},
    { 0x2, 0x9, wxT('K'), wxT("det_maniac"), _("Маньяки")},
    { 0x2, 0xA, wxT('L'), wxT("det_hard"), _("Крутой детектив")},
    { 0x2, 0xB, wxT('M'), wxT("thriller"), _("Триллер")},
    { 0x2, 0x0, wxT('N'), wxT("detective"), _("Детектив")},
    { 0x3, 0x1, wxT('O'), wxT("prose_classic"), _("Классическая проза")},
    { 0x3, 0x2, wxT('P'), wxT("prose_history"), _("Историческая проза")},
    { 0x3, 0x3, wxT('Q'), wxT("prose_contemporary"), _("Современная проза")},
    { 0x3, 0x4, wxT('R'), wxT("prose_counter"), _("Контркультура")},
    { 0x3, 0x5, wxT('S'), wxT("prose_rus_classic"), _("Русская классическая проза")},
    { 0x3, 0x6, wxT('T'), wxT("prose_su_classics"), _("Советская классическая проза")},
    { 0x4, 0x1, wxT('U'), wxT("love_contemporary"), _("Современные любовные романы")},
    { 0x4, 0x2, wxT('V'), wxT("love_history"), _("Исторические любовные романы")},
    { 0x4, 0x3, wxT('W'), wxT("love_detective"), _("Остросюжетные любовные романы")},
    { 0x4, 0x4, wxT('X'), wxT("love_short"), _("Короткие любовные романы")},
    { 0x4, 0x5, wxT('Y'), wxT("love_erotica"), _("Эротика")},
    { 0x5, 0x1, wxT('Z'), wxT("adv_western"), _("Вестерн")},
    { 0x5, 0x2, wxT('a'), wxT("adv_history"), _("Исторические приключения")},
    { 0x5, 0x3, wxT('b'), wxT("adv_indian"), _("Приключения про индейцев")},
    { 0x5, 0x4, wxT('c'), wxT("adv_maritime"), _("Морские приключения")},
    { 0x5, 0x5, wxT('d'), wxT("adv_geo"), _("Путешествия и география")},
    { 0x5, 0x6, wxT('e'), wxT("adv_animal"), _("Природа и животные")},
    { 0x5, 0x0, wxT('f'), wxT("adventure"), _("Приключения")},
    { 0x6, 0x1, wxT('g'), wxT("child_tale"), _("Сказка")},
    { 0x6, 0x2, wxT('h'), wxT("child_verse"), _("Детские стихи")},
    { 0x6, 0x3, wxT('i'), wxT("child_prose"), _("Детскиая проза")},
    { 0x6, 0x4, wxT('j'), wxT("child_sf"), _("Детская фантастика")},
    { 0x6, 0x5, wxT('k'), wxT("child_det"), _("Детские остросюжетные")},
    { 0x6, 0x6, wxT('l'), wxT("child_adv"), _("Детские приключения")},
    { 0x6, 0x7, wxT('m'), wxT("child_education"), _("Детская образовательная литература")},
    { 0x6, 0x0, wxT('n'), wxT("children"), _("Детская литература")},
    { 0x7, 0x1, wxT('o'), wxT("poetry"), _("Поэзия")},
    { 0x7, 0x2, wxT('p'), wxT("dramaturgy"), _("Драматургия")},
    { 0x8, 0x1, wxT('q'), wxT("antique_ant"), _("Античная литература")},
    { 0x8, 0x2, wxT('r'), wxT("antique_european"), _("Европейская старинная литература")},
    { 0x8, 0x3, wxT('s'), wxT("antique_russian"), _("Древнерусская литература")},
    { 0x8, 0x4, wxT('t'), wxT("antique_east"), _("Древневосточная литература")},
    { 0x8, 0x5, wxT('u'), wxT("antique_myths"), _("Мифы. Легенды. Эпос")},
    { 0x8, 0x0, wxT('v'), wxT("antique"), _("Старинная литература")},
    { 0x9, 0x1, wxT('w'), wxT("sci_history"), _("История")},
    { 0x9, 0x2, wxT('x'), wxT("sci_psychology"), _("Психология")},
    { 0x9, 0x3, wxT('y'), wxT("sci_culture"), _("Культурология")},
    { 0x9, 0x4, wxT('z'), wxT("sci_religion"), _("Религиоведение")},
    { 0x9, 0x5, wxT('А'), wxT("sci_philosophy"), _("Философия")},
    { 0x9, 0x6, wxT('Б'), wxT("sci_politics"), _("Политика")},
    { 0x9, 0x7, wxT('В'), wxT("sci_business"), _("Деловая литература")},
    { 0x9, 0x8, wxT('Г'), wxT("sci_juris"), _("Юриспруденция")},
    { 0x9, 0x9, wxT('Д'), wxT("sci_linguistic"), _("Языкознание")},
    { 0x9, 0xA, wxT('Е'), wxT("sci_medicine"), _("Медицина")},
    { 0x9, 0xB, wxT('Ё'), wxT("sci_phys"), _("Физика")},
    { 0x9, 0xC, wxT('Ж'), wxT("sci_math"), _("Математика")},
    { 0x9, 0xD, wxT('З'), wxT("sci_chem"), _("Химия")},
    { 0x9, 0xE, wxT('И'), wxT("sci_biology"), _("Биология")},
    { 0x9, 0xF, wxT('Й'), wxT("sci_tech"), _("Технические науки")},
    { 0x9, 0x0, wxT('К'), wxT("science"), _("Научная литература")},
    { 0xA, 0x1, wxT('Л'), wxT("comp_www"), _("Интернет")},
    { 0xA, 0x2, wxT('М'), wxT("comp_programming"), _("Программирование")},
    { 0xA, 0x3, wxT('Н'), wxT("comp_hard"), _("Компьютерное железо (аппаратное обеспечение)")},
    { 0xA, 0x4, wxT('О'), wxT("comp_soft"), _("Программы")},
    { 0xA, 0x5, wxT('П'), wxT("comp_db"), _("Базы данных")},
    { 0xA, 0x6, wxT('Р'), wxT("comp_osnet"), _("ОС и Сети")},
    { 0xA, 0x0, wxT('С'), wxT("computers"), _("Компьтерная литература")},
    { 0xB, 0x1, wxT('Т'), wxT("ref_encyc"), _("Энциклопедии")},
    { 0xB, 0x2, wxT('У'), wxT("ref_dict"), _("Словари")},
    { 0xB, 0x3, wxT('Ф'), wxT("ref_ref"), _("Справочники")},
    { 0xB, 0x4, wxT('Х'), wxT("ref_guide"), _("Руководства")},
    { 0xB, 0x0, wxT('Ц'), wxT("reference"), _("Справочная литература")},
    { 0xC, 0x1, wxT('Ч'), wxT("nonf_biography"), _("Биографии и Мемуары")},
    { 0xC, 0x2, wxT('Ш'), wxT("nonf_publicism"), _("Публицистика")},
    { 0xC, 0x3, wxT('Щ'), wxT("nonf_criticism"), _("Критика")},
    { 0xC, 0x4, wxT('Ъ'), wxT("design"), _("Искусство и Дизайн")},
    { 0xC, 0x5, wxT('Ы'), wxT("nonfiction"), _("Документальная литература")},
    { 0xD, 0x1, wxT('Ь'), wxT("religion_rel"), _("Религия")},
    { 0xD, 0x2, wxT('Э'), wxT("religion_esoterics"), _("Эзотерика")},
    { 0xD, 0x3, wxT('Ю'), wxT("religion_self"), _("Самосовершенствование")},
    { 0xD, 0x0, wxT('Я'), wxT("religion"), _("Религионая литература")},
    { 0xE, 0x1, wxT('а'), wxT("humor_anecdote"), _("Анекдоты")},
    { 0xE, 0x2, wxT('б'), wxT("humor_prose"), _("Юмористическая проза")},
    { 0xE, 0x3, wxT('в'), wxT("humor_verse"), _("Юмористические стихи")},
    { 0xE, 0x0, wxT('г'), wxT("humor"), _("Юмор")},
    { 0xF, 0x1, wxT('д'), wxT("home_cooking"), _("Кулинария")},
    { 0xF, 0x2, wxT('е'), wxT("home_pets"), _("Домашние животные")},
    { 0xF, 0x3, wxT('ё'), wxT("home_crafts"), _("Хобби и ремесла")},
    { 0xF, 0x4, wxT('ж'), wxT("home_entertain"), _("Развлечения")},
    { 0xF, 0x5, wxT('з'), wxT("home_health"), _("Здоровье")},
    { 0xF, 0x6, wxT('и'), wxT("home_garden"), _("Сад и огород")},
    { 0xF, 0x7, wxT('й'), wxT("home_diy"), _("Сделай сам")},
    { 0xF, 0x8, wxT('к'), wxT("home_sport"), _("Спорт")},
    { 0xF, 0x9, wxT('л'), wxT("home_sex"), _("Эротика, Секс")},
    { 0xF, 0x0, wxT('м'), wxT("home"), _("Домоводство")},
    {   0,   0,        0, wxEmptyString, wxEmptyString},
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
