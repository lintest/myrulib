#include "FbGenres.h"
#include "models/FbGenrTree.h"

//-----------------------------------------------------------------------------
//  FbGenreGroup
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbGenreGroup, wxObject)

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbGenreArray);

//-----------------------------------------------------------------------------
//  FbGenres
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbGenres, wxObject)

wxCriticalSection FbGenres::sm_section;

FbGenreArray FbGenres::sm_groups;

FbStringHash FbGenres::sm_chars;

FbStringHash FbGenres::sm_names;

void FbGenres::Init()
{
	wxCriticalSectionLocker locker(sm_section);

	sm_groups.Empty();
	sm_chars.empty();
	sm_names.empty();

	struct FolderStruct {
		wxChar hi;
		wxString name;
	};

	struct GenreStruct {
		wxChar hi;
		wxChar lo;
		wxString code;
		wxString name;
	};

	const FolderStruct folder_list[] = {
		{ wxT('1'), _("Fiction")},
		{ wxT('2'), _("Mysteries and Thrillers")},
		{ wxT('3'), _("Prose")},
		{ wxT('4'), _("Romance")},
		{ wxT('5'), _("Adventures")},
		{ wxT('6'), _("Children literature")},
		{ wxT('7'), _("Poetry, dramaturgy")},
		{ wxT('8'), _("Antique")},
		{ wxT('9'), _("Science, Education")},
		{ wxT('A'), _("PC, Internet")},
		{ wxT('B'), _("References")},
		{ wxT('C'), _("Documentary literature")},
		{ wxT('D'), _("Religion and Spirituality")},
		{ wxT('E'), _("Humour")},
		{ wxT('F'), _("Home and Family")},
		{ wxT('0'), _("Economy, Business")},
	};

	const GenreStruct genres_list[] = {
		{ wxT('1'), wxT('1'), wxT("sf_history"), _("Alternative History")},
		{ wxT('1'), wxT('2'), wxT("sf_action"), _("Fighting fantasy")},
		{ wxT('1'), wxT('3'), wxT("sf_epic"), _("Epic fantasy")},
		{ wxT('1'), wxT('4'), wxT("sf_heroic"), _("Heroic fantasy")},
		{ wxT('1'), wxT('5'), wxT("sf_detective"), _("Detective Fiction")},
		{ wxT('1'), wxT('6'), wxT("sf_cyberpunk"), _("Cyberpunk")},
		{ wxT('1'), wxT('7'), wxT("sf_space"), _("Space Fiction")},
		{ wxT('1'), wxT('8'), wxT("sf_social"), _("Socio-psychological fiction")},
		{ wxT('1'), wxT('9'), wxT("sf_horror"), _("Horror and Mysticism")},
		{ wxT('1'), wxT('A'), wxT("sf_humor"), _("Humorous Fiction")},
		{ wxT('1'), wxT('B'), wxT("sf_fantasy"), _("Fantasy")},
		{ wxT('1'), wxT('0'), wxT("sf"), _("Science Fiction")},
		{ wxT('2'), wxT('1'), wxT("det_classic"), _("Classic mystery")},
		{ wxT('2'), wxT('2'), wxT("det_police"), _("Police mystery")},
		{ wxT('2'), wxT('3'), wxT("det_action"), _("Action")},
		{ wxT('2'), wxT('4'), wxT("det_irony"), _("Ironic mystery")},
		{ wxT('2'), wxT('5'), wxT("det_history"), _("Historical mystery")},
		{ wxT('2'), wxT('6'), wxT("det_espionage"), _("Spy mystery")},
		{ wxT('2'), wxT('7'), wxT("det_crime"), _("Criminal mystery")},
		{ wxT('2'), wxT('8'), wxT("det_political"), _("Political mystery")},
		{ wxT('2'), wxT('9'), wxT("det_maniac"), _("Maniacs")},
		{ wxT('2'), wxT('A'), wxT("det_hard"), _("Action mystery")},
		{ wxT('2'), wxT('B'), wxT("thriller"), _("Thriller")},
		{ wxT('2'), wxT('0'), wxT("detective"), _("Mystery")},
		{ wxT('3'), wxT('1'), wxT("prose_classic"), _("Classical prose")},
		{ wxT('3'), wxT('2'), wxT("prose_history"), _("Historical prose")},
		{ wxT('3'), wxT('3'), wxT("prose_contemporary"), _("Modern prose")},
		{ wxT('3'), wxT('4'), wxT("prose_counter"), _("Counterculture")},
		{ wxT('3'), wxT('5'), wxT("prose_rus_classic"), _("Russian Classical prose")},
		{ wxT('3'), wxT('6'), wxT("prose_su_classics"), _("Soviet Classical prose")},
		{ wxT('3'), wxT('7'), wxT("prose_military"), _("Prose military")},
		{ wxT('3'), wxT('0'), wxT("prose"), _("Prose")},
		{ wxT('4'), wxT('1'), wxT("love_contemporary"), _("Modern romance novels")},
		{ wxT('4'), wxT('2'), wxT("love_history"), _("Historical romance novels")},
		{ wxT('4'), wxT('3'), wxT("love_detective"), _("Action romance novels")},
		{ wxT('4'), wxT('4'), wxT("love_short"), _("Short romance novels")},
		{ wxT('4'), wxT('5'), wxT("love_erotica"), _("Erotic novels")},
		{ wxT('4'), wxT('6'), wxT("love_sf"), _("Love science fiction")},
		{ wxT('4'), wxT('0'), wxT("love"), _("Love novels")},
		{ wxT('5'), wxT('1'), wxT("adv_western"), _("Western")},
		{ wxT('5'), wxT('2'), wxT("adv_history"), _("Historical adventures")},
		{ wxT('5'), wxT('3'), wxT("adv_indian"), _("Adventures about Indians")},
		{ wxT('5'), wxT('4'), wxT("adv_maritime"), _("Sea adventures")},
		{ wxT('5'), wxT('5'), wxT("adv_geo"), _("Travel and Geography")},
		{ wxT('5'), wxT('6'), wxT("adv_animal"), _("Nature and Animals")},
		{ wxT('5'), wxT('0'), wxT("adventure"), _("Adventures")},
		{ wxT('6'), wxT('1'), wxT("child_tale"), _("Tales")},
		{ wxT('6'), wxT('2'), wxT("child_verse"), _("Children poetry")},
		{ wxT('6'), wxT('3'), wxT("child_prose"), _("Childern prose")},
		{ wxT('6'), wxT('4'), wxT("child_sf"), _("Children fiction")},
		{ wxT('6'), wxT('5'), wxT("child_det"), _("Children action")},
		{ wxT('6'), wxT('6'), wxT("child_adv"), _("Children adventures")},
		{ wxT('6'), wxT('7'), wxT("child_education"), _("Children education")},
		{ wxT('6'), wxT('0'), wxT("children"), _("Children literature")},
		{ wxT('7'), wxT('1'), wxT("poetry"), _("Poetry")},
		{ wxT('7'), wxT('2'), wxT("dramaturgy"), _("Dramaturgy")},
		{ wxT('8'), wxT('1'), wxT("antique_ant"), _("Antique Literature")},
		{ wxT('8'), wxT('2'), wxT("antique_european"), _("European ancient literature")},
		{ wxT('8'), wxT('3'), wxT("antique_russian"), _("Russian ancient literature")},
		{ wxT('8'), wxT('4'), wxT("antique_east"), _("Oriental ancient literature")},
		{ wxT('8'), wxT('5'), wxT("antique_myths"), _("Myths. Legends. Epic.")},
		{ wxT('8'), wxT('0'), wxT("antique"), _("Ancient literature")},
		{ wxT('9'), wxT('1'), wxT("sci_history"), _("History")},
		{ wxT('9'), wxT('2'), wxT("sci_psychology"), _("Psychology")},
		{ wxT('9'), wxT('3'), wxT("sci_culture"), _("Cultural")},
		{ wxT('9'), wxT('4'), wxT("sci_religion"), _("Religious")},
		{ wxT('9'), wxT('5'), wxT("sci_philosophy"), _("Philosophy")},
		{ wxT('9'), wxT('6'), wxT("sci_politics"), _("Politology")},
		{ wxT('9'), wxT('7'), wxT("sci_business"), _("Business literature")},
		{ wxT('9'), wxT('8'), wxT("sci_juris"), _("Law")},
		{ wxT('9'), wxT('9'), wxT("sci_linguistic"), _("Linguistics")},
		{ wxT('9'), wxT('A'), wxT("sci_medicine"), _("Medical")},
		{ wxT('9'), wxT('B'), wxT("sci_phys"), _("Physics")},
		{ wxT('9'), wxT('C'), wxT("sci_math"), _("Mathematics")},
		{ wxT('9'), wxT('D'), wxT("sci_chem"), _("Chemistry")},
		{ wxT('9'), wxT('E'), wxT("sci_biology"), _("Biology")},
		{ wxT('9'), wxT('F'), wxT("sci_tech"), _("Engineering")},
		{ wxT('9'), wxT('0'), wxT("science"), _("Scientific literature")},
		{ wxT('A'), wxT('1'), wxT("comp_www"), _("Internet")},
		{ wxT('A'), wxT('2'), wxT("comp_programming"), _("Programming")},
		{ wxT('A'), wxT('3'), wxT("comp_hard"), _("Computer hardware")},
		{ wxT('A'), wxT('4'), wxT("comp_soft"), _("Software")},
		{ wxT('A'), wxT('5'), wxT("comp_db"), _("Databases")},
		{ wxT('A'), wxT('6'), wxT("comp_osnet"), _("OS and Network")},
		{ wxT('A'), wxT('0'), wxT("computers"), _("Computer literature")},
		{ wxT('B'), wxT('1'), wxT("ref_encyc"), _("Encyclopedias")},
		{ wxT('B'), wxT('2'), wxT("ref_dict"), _("Dictionaries")},
		{ wxT('B'), wxT('3'), wxT("ref_ref"), _("Directories")},
		{ wxT('B'), wxT('4'), wxT("ref_guide"), _("Manuals")},
		{ wxT('B'), wxT('0'), wxT("reference"), _("References")},
		{ wxT('C'), wxT('1'), wxT("nonf_biography"), _("Biographies and Memoirs")},
		{ wxT('C'), wxT('2'), wxT("nonf_publicism"), _("Publicism")},
		{ wxT('C'), wxT('3'), wxT("nonf_criticism"), _("Criticism")},
		{ wxT('C'), wxT('4'), wxT("design"), _("Art and Design")},
		{ wxT('C'), wxT('5'), wxT("nonfiction"), _("Documentary literature")},
		{ wxT('D'), wxT('1'), wxT("religion_rel"), _("Religion")},
		{ wxT('D'), wxT('2'), wxT("religion_esoterics"), _("Esoterica")},
		{ wxT('D'), wxT('3'), wxT("religion_self"), _("Self-improvement")},
		{ wxT('D'), wxT('0'), wxT("religion"), _("Religious Literature")},
		{ wxT('E'), wxT('1'), wxT("humor_anecdote"), _("Jokes")},
		{ wxT('E'), wxT('2'), wxT("humor_prose"), _("Humorous prose")},
		{ wxT('E'), wxT('3'), wxT("humor_verse"), _("Humorous poetry")},
		{ wxT('E'), wxT('0'), wxT("humor"), _("Humour")},
		{ wxT('F'), wxT('1'), wxT("home_cooking"), _("Cooking")},
		{ wxT('F'), wxT('2'), wxT("home_pets"), _("Pets")},
		{ wxT('F'), wxT('3'), wxT("home_crafts"), _("Hobbies and Crafts")},
		{ wxT('F'), wxT('4'), wxT("home_entertain"), _("Entertainment")},
		{ wxT('F'), wxT('5'), wxT("home_health"), _("Health")},
		{ wxT('F'), wxT('6'), wxT("home_garden"), _("Gardening")},
		{ wxT('F'), wxT('7'), wxT("home_diy"), _("Handmade")},
		{ wxT('F'), wxT('8'), wxT("home_sport"), _("Sport")},
		{ wxT('F'), wxT('9'), wxT("home_sex"), _("Erotic, sex")},
		{ wxT('F'), wxT('0'), wxT("home"), _("Household")},
		{ wxT('F'), wxT('A'), wxT("geo_guides"), _("Geo guides")},
		{ wxT('0'), wxT('1'), wxT("job_hunting"), _("Job hunting")},
		{ wxT('0'), wxT('2'), wxT("management"), _("Management")},
		{ wxT('0'), wxT('3'), wxT("marketing"), _("Marketing")},
		{ wxT('0'), wxT('4'), wxT("banking"), _("Banking")},
		{ wxT('0'), wxT('5'), wxT("stock"), _("Stock")},
		{ wxT('0'), wxT('6'), wxT("accounting"), _("Accounting")},
		{ wxT('0'), wxT('7'), wxT("global_economy"), _("Global economy")},
		{ wxT('0'), wxT('0'), wxT("economics"), _("Economics")},
		{ wxT('0'), wxT('8'), wxT("industries"), _("Industries")},
		{ wxT('0'), wxT('9'), wxT("org_behavior"), _("Corporate culture")},
		{ wxT('0'), wxT('A'), wxT("personal_finance"), _("Personal finance")},
		{ wxT('0'), wxT('B'), wxT("real_estate"), _("Real estate")},
		{ wxT('0'), wxT('C'), wxT("popular_business"), _("Popular business")},
		{ wxT('0'), wxT('D'), wxT("small_business"), _("Small business")},
		{ wxT('0'), wxT('E'), wxT("paper_work"), _("Paper work")},
		{ wxT('0'), wxT('F'), wxT("economics_ref"), _("Economics reference book")},
	};

	const size_t folder_count = sizeof(folder_list) / sizeof(FolderStruct);
	const size_t genres_count = sizeof(genres_list) / sizeof(GenreStruct);

	for (size_t i = 0; i < genres_count; i++) {
		const GenreStruct & item = genres_list[i];
		wxString ch; ch << item.hi << item.lo;
		sm_chars[item.code] = ch;
		sm_names[ch] = item.name;
	}

	for (size_t i = 0; i < folder_count; i++) {
		const FolderStruct & folder = folder_list[i];
		wxString code = folder.hi; code << wxT('0');
		FbGenreGroup * group = new FbGenreGroup(code, folder.name);
		for (size_t j=0; j<genres_count; j++) {
			if (genres_list[j].hi == folder.hi) {
				wxString ch; ch << genres_list[j].hi << genres_list[j].lo;
				group->Add(ch);
			}
		}
		sm_groups.Add(group);
	}
}

wxString FbGenres::Char(const wxString &code)
{
	wxCriticalSectionLocker locker(sm_section);
	return sm_chars[code];
}

wxString FbGenres::Name(const wxString &letter)
{
	wxCriticalSectionLocker locker(sm_section);
	return sm_names[letter];
}

wxString FbGenres::DecodeList(const wxString &genres)
{
	wxCriticalSectionLocker locker(sm_section);
	wxString result;
	for (size_t i = 0; i<genres.Len(); i += 2) {
		if (i) result << wxT(',') << wxT(' ');
		wxString code = genres.SubString(i, i + 1);
		result << sm_names[code];
	}
	return result;
}

class FbGenreModel : public FbTreeModel { friend class FbGenres; };

FbModel * FbGenres::CreateModel(const wxString &code)
{
	FbGenreModel * model = new FbGenreModel;
	FbParentData * root = new FbParentData(*model, NULL);
	model->SetRoot(root);

	bool not_found = true;
	size_t count = sm_groups.Count();
	for (size_t i = 0; i < count; i++) {
		FbGenreGroup & group = sm_groups[i];
		FbParentData * parent = new FbGenrParentData(*model, root, group.m_code, group.m_name);
		size_t count = group.m_items.Count();
		for (size_t j = 0; j < count; j++) {
			wxString ch = group.m_items[j];
			new FbGenrChildData(*model, parent, ch, sm_names[ch]);
			if (ch == code) { model->m_position = model->GetRowCount(); not_found = false; }
		}
	}

	FbParentData * parent = new FbGenrParentData(*model, root, wxEmptyString, _("Other"));
	new FbGenrChildData(*model, parent, wxEmptyString, _("No genre"));
	if (not_found) model->m_position = model->GetRowCount();

	return model;
}

void FbGenres::GetNames(FbStringHash & names)
{
	wxCriticalSectionLocker locker(sm_section);
	for (FbStringHash::iterator it = sm_names.begin(); it != sm_names.end(); it++ ) {
		names[it->first] = it->second;
	}
}

//-----------------------------------------------------------------------------
//  FbGenreFunction
//-----------------------------------------------------------------------------

void FbGenreFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	if (ctx.GetArgCount() == 1) ctx.SetResult( DecodeList(ctx.GetString(0)) );
}

wxString FbGenreFunction::DecodeList(const wxString &genres)
{
	wxString result;
	for (size_t i = 0; i<genres.Len(); i += 2) {
		if (i) result << wxT(',') << wxT(' ');
		wxString code = genres.SubString(i, i + 1);
		result << m_names[code];
	}
	return result;
}
