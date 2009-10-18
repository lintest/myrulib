#include "ImpContext.h"
#include "FbManager.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "db/Sequences.h"

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;

WX_DEFINE_OBJARRAY(SequenceArray);

WX_DEFINE_OBJARRAY(AuthorArray);

wxString AuthorItem::GetFullName()
{
    wxString result = last;
    if (!first.IsEmpty()) result += (wxT(" ") + first);
    if (!middle.IsEmpty()) result += (wxT(" ") + middle);
    return result.Trim(false).Trim(true);
}

int AuthorItem::FindAuthor(AuthorItem &author)
{
	wxString search_name = author.GetFullName();
	if (search_name.IsEmpty()) return 0;
	BookInfo::MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? ORDER BY search_name");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, author.first);
	pStatement->SetParamString(2, author.middle);
	pStatement->SetParamString(3, author.last);
	DatabaseResultSet* result = pStatement->ExecuteQuery();
	if (result->Next()) return result->GetResultInt(1);

    wxString letter = search_name.Left(1);
    BookInfo::MakeUpper(letter);
    if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");

	Authors authors(wxGetApp().GetDatabase());
    AuthorsRow * row = authors.New();
    row->id = - BookInfo::NewId(DB_NEW_AUTHOR);
    row->letter = letter;
    row->search_name = search_name;
    row->full_name = author.GetFullName();
    row->first_name = author.first;
    row->middle_name = author.middle;
    row->last_name = author.last;
    row->Save();
    return row->id;
}

int SequenceItem::FindSequence(wxString &name) {

	if (name.IsEmpty()) return 0;

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Sequences sequences(wxGetApp().GetDatabase());
	SequencesRow * seqRow = sequences.Name(name);

	if (!seqRow) {
		seqRow = sequences.New();
		seqRow->id = - BookInfo::NewId(DB_NEW_SEQUENCE);
		seqRow->value = name;
		seqRow->Save();
	}

	return seqRow->id;
}

