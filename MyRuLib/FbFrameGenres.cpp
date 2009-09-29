#include "FbFrameGenres.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbParams.h"
#include "FbManager.h"
#include "BooksPanel.h"
#include "ExternalDlg.h"
#include "FbGenres.h"

BEGIN_EVENT_TABLE(FbFrameGenres, FbFrameBase)
    EVT_MENU(wxID_SAVE, FbFrameGenres::OnExternal)
    EVT_TREE_SEL_CHANGED(ID_GENRES_TREE, FbFrameGenres::OnGenreSelected)
    EVT_UPDATE_UI(ID_EMPTY_BOOKS, FbFrameGenres::OnEmptyBooks)
    EVT_UPDATE_UI(ID_APPEND_BOOK, FbFrameGenres::OnAppendBook)
END_EVENT_TABLE()

FbFrameGenres::FbFrameGenres(wxAuiMDIParentFrame * parent, wxWindowID id,const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameGenres::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	SetMenuBar(CreateMenuBar());

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( toolbar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxSUNKEN_BORDER | wxTR_NO_BUTTONS;
	m_GenresList = new FbTreeListCtrl(splitter, ID_GENRES_TREE, style);
    m_GenresList->AddColumn (_("Список жанров"), 100, wxALIGN_LEFT);
	m_GenresList->SetFocus();
	FbGenres::FillControl(m_GenresList);

	long substyle = wxTR_NO_LINES | wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksPanel.Create(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER, substyle);
	splitter->SplitVertically(m_GenresList, &m_BooksPanel, 160);

    BookListCtrl * booklist = m_BooksPanel.m_BookList;
    booklist->AddColumn (_("Заголовок"), 9, wxALIGN_LEFT);
    booklist->AddColumn (_("Автор"), 6, wxALIGN_LEFT);
    booklist->AddColumn (_("Имя файла"), 4, wxALIGN_LEFT);
    booklist->AddColumn (_("Размер, Кб"), 2, wxALIGN_RIGHT);

	SetSizer( bSizer1 );
	Layout();
}

wxToolBar * FbFrameGenres::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
    wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();
    return toolbar;
}

void FbFrameGenres::OnExternal(wxCommandEvent& event)
{
    ExternalDlg::Execute(this, m_BooksPanel.m_BookList);
}

class GenresThread: public wxThread
{
    public:
        GenresThread(FbFrameGenres * frame, const int code)
			:m_frame(frame), m_booklist(frame->GetBooks()), m_code(code) {};
        virtual void *Entry();
    private:
		static wxCriticalSection sm_queue;
		FbFrameGenres * m_frame;
        BookListCtrl * m_booklist;
        int m_code;
};

class

wxCriticalSection GenresThread::sm_queue;

class FbGenreEvent: public wxUpdateUIEvent
{
	public:
		FbGenreEvent(wxWindowID commandId = 0): wxUpdateUIEvent(commandId) {};
		int id;
};

void * GenresThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	wxString sql = wxT("\
        SELECT books.id, books.title, books.file_name, books.file_type, books.file_size, authors.full_name \
        FROM books LEFT JOIN authors ON books.id_author = authors.id \
        WHERE GENRE(books.genres, ?) \
        ORDER BY books.title, books.id, authors.full_name\
    ");

	FbCommonDatabase database;
	FbGenreFunction function;
    database.CreateFunction(wxT("GENRE"), 2, function);
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, wxString::Format(wxT("%02X"), m_code));
    wxSQLite3ResultSet result = stmt.ExecuteQuery();
    result.NextRow();

	wxUpdateUIEvent event( ID_EMPTY_BOOKS);
	event.SetInt(m_code);
	wxPostEvent(m_frame, event);
	if (event.GetInt() == 0) return NULL;

    while (!result.Eof()) {
        BookTreeItemData data(result);
        wxString full_name = result.GetString(wxT("full_name"));
        do {
            result.NextRow();
            if ( data.GetId() != result.GetInt(wxT("id")) ) break;
            full_name = full_name + wxT(", ") + result.GetString(wxT("full_name"));
        } while (!result.Eof());

		FbGenreEvent event( ID_APPEND_BOOK );
		event.SetString(data.title);
		event.SetText(full_name);
		event.SetInt(m_code);
		event.id = data.GetId();
		wxPostEvent(m_frame, event);
		if ( event.GetInt() == 0 ) break;
/*
        wxTreeItemId item = AppendItem(root, data->title, 0, -1, data);
        wxString full_name = result.GetString(wxT("full_name"));
        SetItemText (item, 1, full_name);
        SetItemText (item, 2, data->file_name);
        SetItemText (item, 3, wxString::Format(wxT("%d "), data->file_size/1024));
*/
    }
    return NULL;
}

wxCriticalSection FbFrameGenres::sm_queue;

void FbFrameGenres::OnGenreSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbGenreData * data = (FbGenreData*) m_GenresList->GetItemData(selected);
		if (data) {
			GenresThread * thread = new GenresThread(this, data->GetCode());
			if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
		}
	}
}

const int FbFrameGenres::GetSelectedCode()
{
	wxTreeItemId selected = m_GenresList->GetSelection();
	if (selected.IsOk()) {
		FbGenreData * data = (FbGenreData*) m_GenresList->GetItemData(selected);
		return data ? data->GetCode() : 0;
	} else
		return 0;
}

void FbFrameGenres::OnEmptyBooks(wxUpdateUIEvent& event)
{
	if ( GetSelectedCode() == event.GetInt() ) {
		m_BooksPanel.m_BookList->Freeze();
		m_BooksPanel.m_BookList->DeleteRoot();
		wxTreeItemId root = m_BooksPanel.m_BookList->AddRoot(wxEmptyString);
		m_BooksPanel.m_BookList->ScrollTo(root);
		m_BooksPanel.m_BookList->Thaw();
	} else {
		event.SetInt(0);
	}
}

void FbFrameGenres::OnAppendBook(wxUpdateUIEvent& event)
{
	if ( GetSelectedCode() == event.GetInt() ) {
		FbGenreEvent & myEvent = (FbGenreEvent&)event;
        BookTreeItemData * data = new BookTreeItemData(myEvent.id);
		m_BooksPanel.m_BookList->Freeze();
		wxTreeItemId root = m_BooksPanel.m_BookList->GetRootItem();
		wxTreeItemId item = m_BooksPanel.m_BookList->AppendItem(root, event.GetString(), 0, -1, data);
		m_BooksPanel.m_BookList->SetItemText(item, 1, event.GetText());
		m_BooksPanel.m_BookList->Thaw();
	} else {
		event.SetInt(0);
	}
}

/*
void FbFrameGenres::FillBooks(const int code)
{
	wxString sql = wxT("\
        SELECT books.id, books.title, books.file_name, books.file_type, books.file_size, authors.full_name \
        FROM books LEFT JOIN authors ON books.id_author = authors.id \
        WHERE GENRE(books.genres, ?) \
        ORDER BY books.title, books.id, authors.full_name\
    ");

    sql.Replace(wxT("/n"), wxT(" "), true);

	FbCommonDatabase database;
	FbGenreFunction function;
    database.CreateFunction(wxT("GENRE"), 2, function);
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, wxString::Format(wxT("%02X"), code));
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

    if (result.NextRow()) {
		m_BooksPanel.m_BookList->FillBooks(result, wxEmptyString);
		m_BooksPanel.m_BookInfo->SetPage(wxEmptyString);
		wxSafeYield();
	} else {
		m_BooksPanel.m_BookList->DeleteRoot();
	}
}

void FbFrameGenres::OnGenreSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbGenreData * data = (FbGenreData*) m_GenresList->GetItemData(selected);
		if (data) FillBooks(data->GetCode());
	}
}
*/
