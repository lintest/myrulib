#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbParams.h"
#include "FbManager.h"
#include "BooksPanel.h"
#include "ExternalDlg.h"
#include "MyRuLibApp.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
    EVT_MENU(wxID_SAVE, FbFrameSearch::OnExternal)
END_EVENT_TABLE()

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameSearch::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	SetMenuBar(CreateMenuBar());

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( toolbar, 0, wxGROW);

	long substyle = wxTR_NO_LINES | wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksPanel.Create(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER, substyle);
	m_BooksPanel.CreateSearchColumns();
	bSizer1->Add( &m_BooksPanel, 1, wxEXPAND, 5 );

	SetSizer( bSizer1 );
	Layout();
}

wxToolBar * FbFrameSearch::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
    wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();
    return toolbar;
}

void FbFrameSearch::OnExternal(wxCommandEvent& event)
{
    ExternalDlg::Execute(m_BooksPanel.m_BookList);
}

void FbFrameSearch::DoSearch(const wxString &title, const wxString &author)
{
    BookListCtrl * booklist = m_BooksPanel.m_BookList;

	booklist->Freeze();

    booklist->DeleteRoot();

    wxString msg = wxString::Format(_T("Поиск: %s %s"), title.c_str(), author.c_str());
    wxTreeItemId root = booklist->AddRoot(msg);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    DatabaseLayer * database = wxGetApp().GetDatabase();
/*
    MyrulibDatabaseLayer::sm_Delta = FbParams::GetValue(DB_BOOKS_COUNT) / 100;
    MyrulibDatabaseLayer::sm_msg = msg;
    MyrulibDatabaseLayer::sm_Progress = 0;
    MyrulibDatabaseLayer::sm_Current = 0;
    {
        wxUpdateUIEvent event( ID_PROGRESS_START );
        event.SetText(msg);
        event.SetInt(100);
        wxPostEvent(wxGetApp().GetTopWindow(), event);
    }
*/
    wxString templ = title;
    templ.Replace(wxT(" "), wxT("%"));
    templ.MakeLower();

	wxString sql = wxT("\
        SELECT books.id, books.title, books.file_name, books.file_type, books.file_size, authors.full_name \
        FROM books \
            LEFT JOIN authors ON books.id_author = authors.id \
        WHERE LOWER(books.title) like ? \
        ORDER BY books.title, books.id, authors.full_name\
        LIMIT 1024 \
    ");
	PreparedStatement* ps = database->PrepareStatement(sql);
	if (!ps) return;
	ps->SetParamString(1, wxT("%") + templ + wxT("%"));
	DatabaseResultSet* result = ps->ExecuteQuery();

    bool notEOF = result && result->Next();
    while (notEOF) {
        BookTreeItemData * data = new BookTreeItemData(result);
        wxTreeItemId item = booklist->AppendItem(root, data->title, 0, -1, data);
        wxString full_name = result->GetResultString(wxT("full_name"));
        booklist->SetItemText (item, 1, full_name);
        booklist->SetItemText (item, 3, data->file_name);
        booklist->SetItemText (item, 4, wxString::Format(wxT("%d "), data->file_size/1024));
        do {
            notEOF = result->Next();
            if ( ! notEOF ) break;
            if ( data->GetId() != result->GetResultInt(wxT("id")) ) break;
            full_name = full_name + wxT(", ") + result->GetResultString(wxT("full_name"));
            booklist->SetItemText (item, 1, full_name);
        } while (true);
    }

	database->CloseResultSet(result);
	database->CloseStatement(ps);

    booklist->ExpandAll(root);
	booklist->Thaw();
	booklist->SetFocus();
/*
    {
        wxUpdateUIEvent event( ID_PROGRESS_FINISH );
        wxPostEvent(wxGetApp().GetTopWindow(), event);
    }
*/
	m_BooksPanel.m_BookInfo->SetPage(wxEmptyString);
}

