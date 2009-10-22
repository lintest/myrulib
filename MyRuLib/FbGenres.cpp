#include "FbGenres.h"

struct GenreStruct {
	char hi;
	char lo;
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
	{ 0xA, _("Компьютеры и Интернет")},
	{ 0xB, _("Справочная литература")},
	{ 0xC, _("Документальная литература")},
	{ 0xD, _("Религия и духовность")},
	{ 0xE, _("Юмор")},
	{ 0xF, _("Домоводство (Дом и семья)")},
	{ 0, wxEmptyString},
};

const GenreStruct genres_list[] = {
	{ 0x1, 0x1, wxT("sf_history"), _("Альтернативная история")},
	{ 0x1, 0x2, wxT("sf_action"), _("Боевая фантастика")},
	{ 0x1, 0x3, wxT("sf_epic"), _("Эпическая фантастика")},
	{ 0x1, 0x4, wxT("sf_heroic"), _("Героическая фантастика")},
	{ 0x1, 0x5, wxT("sf_detective"), _("Детективная фантастика")},
	{ 0x1, 0x6, wxT("sf_cyberpunk"), _("Киберпанк")},
	{ 0x1, 0x7, wxT("sf_space"), _("Космическая фантастика")},
	{ 0x1, 0x8, wxT("sf_social"), _("Социально-психологическая фантастика")},
	{ 0x1, 0x9, wxT("sf_horror"), _("Ужасы и Мистика")},
	{ 0x1, 0xA, wxT("sf_humor"), _("Юмористическая фантастика")},
	{ 0x1, 0xB, wxT("sf_fantasy"), _("Фэнтези")},
	{ 0x1, 0x0, wxT("sf"), _("Научная Фантастика")},
	{ 0x2, 0x1, wxT("det_classic"), _("Классический детектив")},
	{ 0x2, 0x2, wxT("det_police"), _("Полицейский детектив")},
	{ 0x2, 0x3, wxT("det_action"), _("Боевик")},
	{ 0x2, 0x4, wxT("det_irony"), _("Иронический детектив")},
	{ 0x2, 0x5, wxT("det_history"), _("Исторический детектив")},
	{ 0x2, 0x6, wxT("det_espionage"), _("Шпионский детектив")},
	{ 0x2, 0x7, wxT("det_crime"), _("Криминальный детектив")},
	{ 0x2, 0x8, wxT("det_political"), _("Политический детектив")},
	{ 0x2, 0x9, wxT("det_maniac"), _("Маньяки")},
	{ 0x2, 0xA, wxT("det_hard"), _("Крутой детектив")},
	{ 0x2, 0xB, wxT("thriller"), _("Триллер")},
	{ 0x2, 0x0, wxT("detective"), _("Детектив")},
	{ 0x3, 0x1, wxT("prose_classic"), _("Классическая проза")},
	{ 0x3, 0x2, wxT("prose_history"), _("Историческая проза")},
	{ 0x3, 0x3, wxT("prose_contemporary"), _("Современная проза")},
	{ 0x3, 0x4, wxT("prose_counter"), _("Контркультура")},
	{ 0x3, 0x5, wxT("prose_rus_classic"), _("Русская классическая проза")},
	{ 0x3, 0x6, wxT("prose_su_classics"), _("Советская классическая проза")},
	{ 0x4, 0x1, wxT("love_contemporary"), _("Современные любовные романы")},
	{ 0x4, 0x2, wxT("love_history"), _("Исторические любовные романы")},
	{ 0x4, 0x3, wxT("love_detective"), _("Остросюжетные любовные романы")},
	{ 0x4, 0x4, wxT("love_short"), _("Короткие любовные романы")},
	{ 0x4, 0x5, wxT("love_erotica"), _("Эротика")},
	{ 0x5, 0x1, wxT("adv_western"), _("Вестерн")},
	{ 0x5, 0x2, wxT("adv_history"), _("Исторические приключения")},
	{ 0x5, 0x3, wxT("adv_indian"), _("Приключения про индейцев")},
	{ 0x5, 0x4, wxT("adv_maritime"), _("Морские приключения")},
	{ 0x5, 0x5, wxT("adv_geo"), _("Путешествия и география")},
	{ 0x5, 0x6, wxT("adv_animal"), _("Природа и животные")},
	{ 0x5, 0x0, wxT("adventure"), _("Приключения")},
	{ 0x6, 0x1, wxT("child_tale"), _("Сказка")},
	{ 0x6, 0x2, wxT("child_verse"), _("Детские стихи")},
	{ 0x6, 0x3, wxT("child_prose"), _("Детскиая проза")},
	{ 0x6, 0x4, wxT("child_sf"), _("Детская фантастика")},
	{ 0x6, 0x5, wxT("child_det"), _("Детские остросюжетные")},
	{ 0x6, 0x6, wxT("child_adv"), _("Детские приключения")},
	{ 0x6, 0x7, wxT("child_education"), _("Детская образовательная литература")},
	{ 0x6, 0x0, wxT("children"), _("Детская литература")},
	{ 0x7, 0x1, wxT("poetry"), _("Поэзия")},
	{ 0x7, 0x2, wxT("dramaturgy"), _("Драматургия")},
	{ 0x8, 0x1, wxT("antique_ant"), _("Античная литература")},
	{ 0x8, 0x2, wxT("antique_european"), _("Европейская старинная литература")},
	{ 0x8, 0x3, wxT("antique_russian"), _("Древнерусская литература")},
	{ 0x8, 0x4, wxT("antique_east"), _("Древневосточная литература")},
	{ 0x8, 0x5, wxT("antique_myths"), _("Мифы. Легенды. Эпос")},
	{ 0x8, 0x0, wxT("antique"), _("Старинная литература")},
	{ 0x9, 0x1, wxT("sci_history"), _("История")},
	{ 0x9, 0x2, wxT("sci_psychology"), _("Психология")},
	{ 0x9, 0x3, wxT("sci_culture"), _("Культурология")},
	{ 0x9, 0x4, wxT("sci_religion"), _("Религиоведение")},
	{ 0x9, 0x5, wxT("sci_philosophy"), _("Философия")},
	{ 0x9, 0x6, wxT("sci_politics"), _("Политика")},
	{ 0x9, 0x7, wxT("sci_business"), _("Деловая литература")},
	{ 0x9, 0x8, wxT("sci_juris"), _("Юриспруденция")},
	{ 0x9, 0x9, wxT("sci_linguistic"), _("Языкознание")},
	{ 0x9, 0xA, wxT("sci_medicine"), _("Медицина")},
	{ 0x9, 0xB, wxT("sci_phys"), _("Физика")},
	{ 0x9, 0xC, wxT("sci_math"), _("Математика")},
	{ 0x9, 0xD, wxT("sci_chem"), _("Химия")},
	{ 0x9, 0xE, wxT("sci_biology"), _("Биология")},
	{ 0x9, 0xF, wxT("sci_tech"), _("Технические науки")},
	{ 0x9, 0x0, wxT("science"), _("Научная литература")},
	{ 0xA, 0x1, wxT("comp_www"), _("Интернет")},
	{ 0xA, 0x2, wxT("comp_programming"), _("Программирование")},
	{ 0xA, 0x3, wxT("comp_hard"), _("Компьютерное железо (аппаратное обеспечение)")},
	{ 0xA, 0x4, wxT("comp_soft"), _("Программы")},
	{ 0xA, 0x5, wxT("comp_db"), _("Базы данных")},
	{ 0xA, 0x6, wxT("comp_osnet"), _("ОС и Сети")},
	{ 0xA, 0x0, wxT("computers"), _("Компьтерная литература")},
	{ 0xB, 0x1, wxT("ref_encyc"), _("Энциклопедии")},
	{ 0xB, 0x2, wxT("ref_dict"), _("Словари")},
	{ 0xB, 0x3, wxT("ref_ref"), _("Справочники")},
	{ 0xB, 0x4, wxT("ref_guide"), _("Руководства")},
	{ 0xB, 0x0, wxT("reference"), _("Справочная литература")},
	{ 0xC, 0x1, wxT("nonf_biography"), _("Биографии и Мемуары")},
	{ 0xC, 0x2, wxT("nonf_publicism"), _("Публицистика")},
	{ 0xC, 0x3, wxT("nonf_criticism"), _("Критика")},
	{ 0xC, 0x4, wxT("design"), _("Искусство и Дизайн")},
	{ 0xC, 0x5, wxT("nonfiction"), _("Документальная литература")},
	{ 0xD, 0x1, wxT("religion_rel"), _("Религия")},
	{ 0xD, 0x2, wxT("religion_esoterics"), _("Эзотерика")},
	{ 0xD, 0x3, wxT("religion_self"), _("Самосовершенствование")},
	{ 0xD, 0x0, wxT("religion"), _("Религионая литература")},
	{ 0xE, 0x1, wxT("humor_anecdote"), _("Анекдоты")},
	{ 0xE, 0x2, wxT("humor_prose"), _("Юмористическая проза")},
	{ 0xE, 0x3, wxT("humor_verse"), _("Юмористические стихи")},
	{ 0xE, 0x0, wxT("humor"), _("Юмор")},
	{ 0xF, 0x1, wxT("home_cooking"), _("Кулинария")},
	{ 0xF, 0x2, wxT("home_pets"), _("Домашние животные")},
	{ 0xF, 0x3, wxT("home_crafts"), _("Хобби и ремесла")},
	{ 0xF, 0x4, wxT("home_entertain"), _("Развлечения")},
	{ 0xF, 0x5, wxT("home_health"), _("Здоровье")},
	{ 0xF, 0x6, wxT("home_garden"), _("Сад и огород")},
	{ 0xF, 0x7, wxT("home_diy"), _("Сделай сам")},
	{ 0xF, 0x8, wxT("home_sport"), _("Спорт")},
	{ 0xF, 0x9, wxT("home_sex"), _("Эротика, Секс")},
	{ 0xF, 0x0, wxT("home"), _("Домоводство")},
	{ 0x0},
};

const size_t genres_count = sizeof(genres_list) / sizeof(GenreStruct);

wxString FbGenres::Char(const wxString &code)
{
	for (size_t i=0; genres_list[i].hi; i++)
		if (genres_list[i].code == code)
			return wxString::Format(wxT("%X%X"), genres_list[i].hi, genres_list[i].lo);
	return wxEmptyString;
}

wxString FbGenres::Name(const wxString &letter)
{
	long code = 0;
	if (letter.ToLong(&code, 16)) {
		for (size_t i=0; genres_list[i].hi; i++)
			if (genres_list[i].hi*16 + genres_list[i].lo == code)
				return genres_list[i].name;
	}
	return wxEmptyString;
}

wxString FbGenres::Name(const int code)
{
	for (size_t i=0; genres_list[i].hi; i++)
		if (genres_list[i].hi*16 + genres_list[i].lo == code)
			return genres_list[i].name;
	return wxEmptyString;
}

wxString FbGenres::DecodeList(const wxString &genres)
{
	wxString result;
	for (size_t i = 0; i<genres.Len()/2; i++) {
		if (!result.IsEmpty()) result += wxT(", ");
		wxString code = genres.SubString(i*2, i*2+1);
		result +=  FbGenres::Name( code );
	}
	return result;
}

void FbGenres::FillControl(wxTreeListCtrl * control)
{
	control->Freeze();
	wxTreeItemId root = control->AddRoot(wxEmptyString);

	for (size_t i=0; hi_genres[i].hi; i++) {
		wxTreeItemId parent = control->AppendItem(root, hi_genres[i].name);
		control->SetItemBold(parent, true);
		for (size_t j=0; genres_list[j].hi; j++) {
			if (genres_list[j].hi == hi_genres[i].hi) {
				int code = genres_list[j].hi * 0x10 + genres_list[j].lo;
				control->AppendItem(parent, genres_list[j].name, -1, -1, new FbGenreData(code));
			}
		}
		control->Expand(parent);
	}
	control->ExpandAll(root);
	control->Thaw();
	control->Update();
}
