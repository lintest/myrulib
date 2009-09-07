#include "FbAuthorList.h"
#include <DatabaseLayer.h>
#include "RecordIDClientData.h"
#include "ImpContext.h"
#include "FbManager.h"
#include "MyRuLibApp.h"

void FbAuthorList::FillAuthorsChar(const wxChar & findLetter)
{
    const wxString orderBy = wxT("search_name");
	wxString findText = findLetter;
    const wxString whereClause = wxString::Format(wxT("letter = '%s'"), findText.c_str());

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    DatabaseLayer * database = wxGetApp().GetDatabase();

	wxString sql = wxT("SELECT id, first_name, middle_name, last_name FROM authors WHERE letter=? ORDER BY search_name");
	PreparedStatement* ps = database->PrepareStatement(sql);
	ps->SetParamString(1, findLetter);
	DatabaseResultSet* result = ps->ExecuteQuery();
	FillAuthors(result);
	database->CloseResultSet(result);
	database->CloseStatement(ps);
}

void FbAuthorList::FillAuthorsText(const wxString & findText)
{
    const wxString orderBy = wxT("search_name");
    wxString text = findText;
    BookInfo::MakeLower(text);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    DatabaseLayer * database = wxGetApp().GetDatabase();

	wxString sql = wxT("SELECT id, first_name, middle_name, last_name FROM authors WHERE search_name like ? ORDER BY search_name");
	PreparedStatement* ps = database->PrepareStatement(sql);
	ps->SetParamString(1, text + wxT("%"));
	DatabaseResultSet* result = ps->ExecuteQuery();
	FillAuthors(result);
	database->CloseResultSet(result);
	database->CloseStatement(ps);
}

void FbAuthorList::FillAuthors(DatabaseResultSet* result)
{
	Freeze();
	Clear();

    while(result && result->Next()){
        int id = result->GetResultInt(wxT("id"));
        AuthorItem item;
        item.first  = result->GetResultString(wxT("first_name"));
        item.middle = result->GetResultString(wxT("middle_name"));
        item.last   = result->GetResultString(wxT("last_name"));
        Append(item.GetFullName(), new RecordIDClientData(id));
    }

	Thaw();
}
