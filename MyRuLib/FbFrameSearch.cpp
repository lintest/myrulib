#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbParams.h"
#include "FbManager.h"
#include "BooksPanel.h"
#include "ExternalDlg.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
    EVT_MENU(wxID_SAVE, FbFrameSearch::OnExternal)
END_EVENT_TABLE()

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString & title)
    :FbFrameBase(parent, wxID_ANY, title)
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
	bSizer1->Add( &m_BooksPanel, 1, wxEXPAND, 5 );

    BookListCtrl * booklist = m_BooksPanel.m_BookList;
    booklist->AddColumn (_T("Заголовок"), 9, wxALIGN_LEFT);
    booklist->AddColumn (_T("Автор"), 6, wxALIGN_LEFT);
    booklist->AddColumn (_T("№"), 1, wxALIGN_LEFT);
    booklist->AddColumn (_T("Имя файла"), 4, wxALIGN_LEFT);
    booklist->AddColumn (_T("Размер, Кб"), 2, wxALIGN_RIGHT);

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
    ExternalDlg::Execute(this, m_BooksPanel.m_BookList);
}

class SearchThread: public wxThread
{
    public:
        SearchThread(FbFrameSearch * frame, const wxString &title)
			:m_frame(frame), m_booklist(frame->GetBooks()), m_title(title) {};
        virtual void *Entry();
    private:
		void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
    private:
		FbFrameSearch * m_frame;
        BookListCtrl * m_booklist;
        wxString m_title;
};

void * SearchThread::Entry()
{
	wxString msg = wxString::Format(_("Поиск: «%s»"), m_title.c_str());

    wxString templ = wxT('%') + m_title + wxT('%');
    templ.Replace(wxT(" "), wxT("%"));
    BookInfo::MakeLower(templ);

	wxString sql = wxT("\
        SELECT books.id, books.title, books.file_name, books.file_type, books.file_size, authors.full_name \
        FROM books \
            LEFT JOIN authors ON books.id_author = authors.id \
        WHERE LOWER(books.title) like ? \
        ORDER BY books.title, books.id, authors.full_name\
        LIMIT 1024 \
    ");

    sql.Replace(wxT("/n"), wxT(" "), true);

	FbCommonDatabase database;
	FbLowerFunction lower;
    database.CreateFunction(wxT("LOWER"), 1, lower);
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, templ);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

    if (!result.NextRow()) {
    	wxString text = wxString::Format(_("Ничего не найдено по шаблону «%s»."), m_title.c_str());
    	wxMessageBox(text, _("Поиск"));
    	m_frame->Close();
    	return NULL;
	}

	FillBooks(result, msg);
	return NULL;
}

void SearchThread::FillBooks(wxSQLite3ResultSet & result, const wxString &caption)
{
	m_booklist->Freeze();
    m_booklist->DeleteRoot();
    wxTreeItemId root = m_booklist->AddRoot(caption);

    while (!result.Eof()) {
        BookTreeItemData * data = new BookTreeItemData(result);
        wxTreeItemId item = m_booklist->AppendItem(root, data->title, 0, -1, data);
        wxString full_name = result.GetString(wxT("full_name"));
        m_booklist->SetItemText (item, 1, full_name);
        m_booklist->SetItemText (item, 3, data->file_name);
        m_booklist->SetItemText (item, 4, wxString::Format(wxT("%d "), data->file_size/1024));
        do {
            result.NextRow();
            if ( data->GetId() != result.GetInt(wxT("id")) ) break;
            full_name = full_name + wxT(", ") + result.GetString(wxT("full_name"));
            m_booklist->SetItemText (item, 1, full_name);
        } while (!result.Eof());
    }

    m_booklist->ExpandAll(root);
	m_booklist->Thaw();
	m_booklist->SetFocus();
}

void FbFrameSearch::Execute(wxAuiMDIParentFrame * parent, const wxString &title)
{
    if ( title.IsEmpty() ) return;
    wxLogInfo(_("Search title: %s"), title.c_str());

	wxString msg = wxString::Format(_("Поиск: «%s»"), title.c_str());
	FbFrameSearch * frame = new FbFrameSearch(parent, msg);
	frame->Show(false);

	SearchThread * thread = new SearchThread(frame, title);
	if ( thread->Create() != wxTHREAD_NO_ERROR ) {
		wxLogError(wxT("Can't create search thread: ") + title);
		return;
	}
	thread->Run();
}


