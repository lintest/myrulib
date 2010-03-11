#include "FbGenres.h"
#include "FbMasterData.h"

void FbGenres::Do(ID id, const void * value, void * result)
{
	struct FolderStruct {
		char hi;
		wxString name;
	};

	struct GenreStruct {
		char hi;
		char lo;
		wxString code;
		wxString name;
	};

	const FolderStruct folder_list[] = {
		{ 0x1, _("Fiction")},
		{ 0x2, _("Mysteries and Thrillers")},
		{ 0x3, _("Prose")},
		{ 0x4, _("Romance")},
		{ 0x5, _("Adventures")},
		{ 0x6, _("Children literature")},
		{ 0x7, _("Poetry, dramaturgy")},
		{ 0x8, _("Antique")},
		{ 0x9, _("Science, Education")},
		{ 0xA, _("PC, Internet")},
		{ 0xB, _("References")},
		{ 0xC, _("Documentary literature")},
		{ 0xD, _("Religion and Spirituality")},
		{ 0xE, _("Humour")},
		{ 0xF, _("Home and Family")},
	};

	const GenreStruct genres_list[] = {
		{ 0x1, 0x1, wxT("sf_history"), _("Alternative History")},
		{ 0x1, 0x2, wxT("sf_action"), _("Fighting fantasy")},
		{ 0x1, 0x3, wxT("sf_epic"), _("Epic fantasy")},
		{ 0x1, 0x4, wxT("sf_heroic"), _("Heroic fantasy")},
		{ 0x1, 0x5, wxT("sf_detective"), _("Detective Fiction")},
		{ 0x1, 0x6, wxT("sf_cyberpunk"), _("Cyberpunk")},
		{ 0x1, 0x7, wxT("sf_space"), _("Space Fiction")},
		{ 0x1, 0x8, wxT("sf_social"), _("Socio-psychological fiction")},
		{ 0x1, 0x9, wxT("sf_horror"), _("Horror and Mysticism")},
		{ 0x1, 0xA, wxT("sf_humor"), _("Humorous Fiction")},
		{ 0x1, 0xB, wxT("sf_fantasy"), _("Fantasy")},
		{ 0x1, 0x0, wxT("sf"), _("Science Fiction")},
		{ 0x2, 0x1, wxT("det_classic"), _("Classic mystery")},
		{ 0x2, 0x2, wxT("det_police"), _("Police mystery")},
		{ 0x2, 0x3, wxT("det_action"), _("Action")},
		{ 0x2, 0x4, wxT("det_irony"), _("Ironic mystery")},
		{ 0x2, 0x5, wxT("det_history"), _("Historical mystery")},
		{ 0x2, 0x6, wxT("det_espionage"), _("Spy mystery")},
		{ 0x2, 0x7, wxT("det_crime"), _("Criminal mystery")},
		{ 0x2, 0x8, wxT("det_political"), _("Political mystery")},
		{ 0x2, 0x9, wxT("det_maniac"), _("Maniacs")},
		{ 0x2, 0xA, wxT("det_hard"), _("Action mystery")},
		{ 0x2, 0xB, wxT("thriller"), _("Thriller")},
		{ 0x2, 0x0, wxT("detective"), _("Mystery")},
		{ 0x3, 0x1, wxT("prose_classic"), _("Classical prose")},
		{ 0x3, 0x2, wxT("prose_history"), _("Historical prose")},
		{ 0x3, 0x3, wxT("prose_contemporary"), _("Modern prose")},
		{ 0x3, 0x4, wxT("prose_counter"), _("Counterculture")},
		{ 0x3, 0x5, wxT("prose_rus_classic"), _("Russian Classical prose")},
		{ 0x3, 0x6, wxT("prose_su_classics"), _("Soviet Classical prose")},
		{ 0x4, 0x1, wxT("love_contemporary"), _("Modern romance novels")},
		{ 0x4, 0x2, wxT("love_history"), _("historical romance novels")},
		{ 0x4, 0x3, wxT("love_detective"), _("Action romance novels")},
		{ 0x4, 0x4, wxT("love_short"), _("Short romance novels")},
		{ 0x4, 0x5, wxT("love_erotica"), _("Erotic novels")},
		{ 0x5, 0x1, wxT("adv_western"), _("Western")},
		{ 0x5, 0x2, wxT("adv_history"), _("Historical adventures")},
		{ 0x5, 0x3, wxT("adv_indian"), _("Adventures about Indians")},
		{ 0x5, 0x4, wxT("adv_maritime"), _("Sea adventures")},
		{ 0x5, 0x5, wxT("adv_geo"), _("Travel and Geography")},
		{ 0x5, 0x6, wxT("adv_animal"), _("Nature and Animals")},
		{ 0x5, 0x0, wxT("adventure"), _("Adventures")},
		{ 0x6, 0x1, wxT("child_tale"), _("Tales")},
		{ 0x6, 0x2, wxT("child_verse"), _("Children poetry")},
		{ 0x6, 0x3, wxT("child_prose"), _("Childern prose")},
		{ 0x6, 0x4, wxT("child_sf"), _("Children fiction")},
		{ 0x6, 0x5, wxT("child_det"), _("Children action")},
		{ 0x6, 0x6, wxT("child_adv"), _("Children adventures")},
		{ 0x6, 0x7, wxT("child_education"), _("Children education")},
		{ 0x6, 0x0, wxT("children"), _("Children literature")},
		{ 0x7, 0x1, wxT("poetry"), _("Poetry")},
		{ 0x7, 0x2, wxT("dramaturgy"), _("Dramaturgy")},
		{ 0x8, 0x1, wxT("antique_ant"), _("Antique Literature")},
		{ 0x8, 0x2, wxT("antique_european"), _("European ancient literature")},
		{ 0x8, 0x3, wxT("antique_russian"), _("Russian ancient literature")},
		{ 0x8, 0x4, wxT("antique_east"), _("Oriental ancient literature")},
		{ 0x8, 0x5, wxT("antique_myths"), _("Medical")},
		{ 0x8, 0x0, wxT("antique"), _("Ancient literature")},
		{ 0x9, 0x1, wxT("sci_history"), _("History")},
		{ 0x9, 0x2, wxT("sci_psychology"), _("Psychology")},
		{ 0x9, 0x3, wxT("sci_culture"), _("Cultural")},
		{ 0x9, 0x4, wxT("sci_religion"), _("Religious")},
		{ 0x9, 0x5, wxT("sci_philosophy"), _("Philosophy")},
		{ 0x9, 0x6, wxT("sci_politics"), _("Politology")},
		{ 0x9, 0x7, wxT("sci_business"), _("Business literature")},
		{ 0x9, 0x8, wxT("sci_juris"), _("Law")},
		{ 0x9, 0x9, wxT("sci_linguistic"), _("Linguistics")},
		{ 0x9, 0xA, wxT("sci_medicine"), _("Medical")},
		{ 0x9, 0xB, wxT("sci_phys"), _("Physics")},
		{ 0x9, 0xC, wxT("sci_math"), _("Mathematics")},
		{ 0x9, 0xD, wxT("sci_chem"), _("Chemistry")},
		{ 0x9, 0xE, wxT("sci_biology"), _("Biology")},
		{ 0x9, 0xF, wxT("sci_tech"), _("Engineering")},
		{ 0x9, 0x0, wxT("science"), _("Scientific literature")},
		{ 0xA, 0x1, wxT("comp_www"), _("Health")},
		{ 0xA, 0x2, wxT("comp_programming"), _("Programming")},
		{ 0xA, 0x3, wxT("comp_hard"), _("Computer hardware")},
		{ 0xA, 0x4, wxT("comp_soft"), _("Software")},
		{ 0xA, 0x5, wxT("comp_db"), _("Databases")},
		{ 0xA, 0x6, wxT("comp_osnet"), _("OS and Network")},
		{ 0xA, 0x0, wxT("computers"), _("Computer literature")},
		{ 0xB, 0x1, wxT("ref_encyc"), _("Encyclopedias")},
		{ 0xB, 0x2, wxT("ref_dict"), _("Dictionaries")},
		{ 0xB, 0x3, wxT("ref_ref"), _("Directories")},
		{ 0xB, 0x4, wxT("ref_guide"), _("Manuals")},
		{ 0xB, 0x0, wxT("reference"), _("References")},
		{ 0xC, 0x1, wxT("nonf_biography"), _("Biographies and Memoirs")},
		{ 0xC, 0x2, wxT("nonf_publicism"), _("Publicism")},
		{ 0xC, 0x3, wxT("nonf_criticism"), _("Criticism")},
		{ 0xC, 0x4, wxT("design"), _("Art and Design")},
		{ 0xC, 0x5, wxT("nonfiction"), _("Documentary literature")},
		{ 0xD, 0x1, wxT("religion_rel"), _("Religion")},
		{ 0xD, 0x2, wxT("religion_esoterics"), _("Esoterica")},
		{ 0xD, 0x3, wxT("religion_self"), _("Self-improvement")},
		{ 0xD, 0x0, wxT("religion"), _("Religious Literature")},
		{ 0xE, 0x1, wxT("humor_anecdote"), _("Jokes")},
		{ 0xE, 0x2, wxT("humor_prose"), _("Humorous prose")},
		{ 0xE, 0x3, wxT("humor_verse"), _("Humorous poetry")},
		{ 0xE, 0x0, wxT("humor"), _("Humour")},
		{ 0xF, 0x1, wxT("home_cooking"), _("Cooking")},
		{ 0xF, 0x2, wxT("home_pets"), _("Pets")},
		{ 0xF, 0x3, wxT("home_crafts"), _("Hobbies and Crafts")},
		{ 0xF, 0x4, wxT("home_entertain"), _("Entertainment")},
		{ 0xF, 0x5, wxT("home_health"), _("Health")},
		{ 0xF, 0x6, wxT("home_garden"), _("Gardening")},
		{ 0xF, 0x7, wxT("home_diy"), _("Handmade")},
		{ 0xF, 0x8, wxT("home_sport"), _("Sport")},
		{ 0xF, 0x9, wxT("home_sex"), _("Erotic, sex")},
		{ 0xF, 0x0, wxT("home"), _("Household")},
	};

	const size_t folder_count = sizeof(folder_list) / sizeof(FolderStruct);
	const size_t genres_count = sizeof(genres_list) / sizeof(GenreStruct);

	switch (id) {
		case ID_CHAR: {
			const wxString * code = (wxString*) value;
			wxString * res = (wxString*) result;
			for (size_t i=0; i<genres_count; i++) {
				if (genres_list[i].code == *code) {
					*res = wxString::Format(wxT("%X%X"), genres_list[i].hi, genres_list[i].lo);
					return;
				}
			}
		} break;
		case ID_NAME: {
			long code = 0;
			const wxString * letter = (wxString*) value;
			wxString * res = (wxString*) result;
			if (letter->ToLong(&code, 16)) {
				for (size_t i=0; i<genres_count; i++)
					if (genres_list[i].hi*16 + genres_list[i].lo == code) {
						*res = genres_list[i].name;
						return;
					}
			}
		} break;
		case ID_DECODE: {
			const int code = *(int*)value;
			wxString * res = (wxString*) result;
			for (size_t i=0; i<genres_count; i++)
				if (genres_list[i].hi*16 + genres_list[i].lo == code) {
					*res = genres_list[i].name;
					return;
				}
		} break;
		case ID_FILL: {
			wxTreeListCtrl * control = (wxTreeListCtrl*) result;

			control->Freeze();
			control->DeleteRoot();
			wxTreeItemId root = control->AddRoot(wxEmptyString);
			for (size_t i=0; i<folder_count; i++) {
				wxTreeItemId parent = control->AppendItem(root, folder_list[i].name);
				control->SetItemBold(parent, true);
				for (size_t j=0; j<genres_count; j++) {
					if (genres_list[j].hi == folder_list[i].hi) {
						int code = genres_list[j].hi * 0x10 + genres_list[j].lo;
						control->AppendItem(parent, genres_list[j].name, -1, -1, new FbMasterGenre(code));
					}
				}
				control->Expand(parent);
			}
			control->ExpandAll(root);
			control->Thaw();
			control->Update();
		} break;
	}
}

wxString FbGenres::Char(const wxString &code)
{
	wxString result;
	Do(ID_CHAR, &code, &result);
	return result;
}

wxString FbGenres::Name(const wxString &letter)
{
	wxString result;
	Do(ID_NAME, &letter, &result);
	return result;
}

wxString FbGenres::Name(const int code)
{
	wxString result;
	Do(ID_DECODE, &code, &result);
	return result;
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
	Do(ID_FILL, NULL, control);
}
