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
	m_GenresList = new BookListCtrl(splitter, ID_GENRES_TREE, style);
    m_GenresList->AddColumn (_("Список жанров"), 100, wxALIGN_LEFT);
	m_GenresList->SetFocus();
	FbGenres::FillControl(m_GenresList);

	long substyle = wxTR_NO_LINES | wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksPanel.Create(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER, substyle);
	splitter->SplitVertically(m_GenresList, &m_BooksPanel, 160);

    BookListCtrl * booklist = m_BooksPanel.m_BookList;
    booklist->AddColumn (_("Заголовок"), 9, wxALIGN_LEFT);
    booklist->AddColumn (_("Автор"), 6, wxALIGN_LEFT);
    booklist->AddColumn (_("№"), 1, wxALIGN_LEFT);
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
/*
class GenresThread: public wxThread
{
    public:
        GenresThread(FbFrameGenres * frame, const wxString &code)
			:m_frame(frame), m_booklist(frame->GetBooks()), m_code(code) {};
        virtual void *Entry();
    private:
		static wxCriticalSection sm_queue;
		FbFrameGenres * m_frame;
        BookListCtrl * m_booklist;
        wxString m_code;
};

wxCriticalSection GenresThread::sm_queue;

void * GenresThread::Entry()
{}

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

*/

void FbFrameGenres::FillBooks(const wxString & code)
{
	wxSafeYield();

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
    stmt.Bind(1, code);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

    if (result.NextRow()) {
		m_BooksPanel.m_BookList->FillBooks(result, wxEmptyString);
		wxSafeYield();
	} else {
		m_BooksPanel.m_BookList->DeleteRoot();
	}
}

void FbFrameGenres::OnGenreSelected(wxTreeEvent & event)
{
	return;
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbGenreData * data = (FbGenreData*) m_GenresList->GetItemData(selected);
		if (data) FillBooks(data->GetCode());
	}
}

