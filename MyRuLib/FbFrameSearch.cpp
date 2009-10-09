#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "BooksPanel.h"
#include "FbFrameBaseThread.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
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

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksPanel.Create(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER, substyle);
	bSizer1->Add( &m_BooksPanel, 1, wxEXPAND, 5 );

    m_BooksPanel.CreateColumns(GetListMode(FB_MODE_SEARCH));

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

class FrameSearchThread: public FbFrameBaseThread
{
    public:
        FrameSearchThread(FbFrameBase * frame, FbListMode mode, const wxString &title)
			:FbFrameBaseThread(frame, mode), m_title(title) {};
        virtual void *Entry();
    private:
        wxString m_title;
};

void * FrameSearchThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	EmptyBooks();

	wxString condition = wxT("LOWER(books.title) like ?");
	wxString sql = GetSQL(condition);

    wxString text = wxT('%') + m_title + wxT('%');
    text.Replace(wxT(" "), wxT("%"));
    text.Replace(wxT("?"), wxT("_"));
    text.Replace(wxT("*"), wxT("%"));
    BookInfo::MakeLower(text);

	try {
		FbCommonDatabase database;
		FbLowerFunction lower;
		database.CreateFunction(wxT("LOWER"), 1, lower);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, text);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (result.Eof()) {
			wxString text = wxString::Format(_("Ничего не найдено по шаблону «%s»."), m_title.c_str());
			wxMessageBox(text, _("Поиск"));
			m_frame->Close();
			return NULL;
		}
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}


	return NULL;
}

void FbFrameSearch::Execute(wxAuiMDIParentFrame * parent, const wxString &title)
{
    if ( title.IsEmpty() ) return;
    wxLogInfo(_("Search title: %s"), title.c_str());

	wxString msg = wxString::Format(_("Поиск: «%s»"), title.c_str());
	FbFrameSearch * frame = new FbFrameSearch(parent, msg);
    frame->m_title = title;
	frame->Update();

	frame->UpdateBooklist();
}

void FbFrameSearch::UpdateBooklist()
{
	wxThread * thread = new FrameSearchThread(this, m_BooksPanel.GetListMode(), m_title);
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}
