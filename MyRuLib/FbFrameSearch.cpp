#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include <wx/mimetype.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "FbFrameBaseThread.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
	EVT_COMMAND(ID_FOUND_NOTHING, fbEVT_BOOK_ACTION, FbFrameSearch::OnFoundNothing)
END_EVENT_TABLE()

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString & title)
	:FbFrameBase(parent, ID_FRAME_SEARCH, title)
{
	CreateControls();
}

void FbFrameSearch::CreateControls()
{
	SetMenuBar(new FbFrameBaseMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( toolbar, 0, wxGROW);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(this, substyle);
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

class FrameSearchThread: public FbFrameBaseThread
{
	public:
		FrameSearchThread(FbFrameBase * frame, FbListMode mode, const wxString &title)
			:FbFrameBaseThread(frame, mode), m_title(title) {};
		virtual void *Entry();
	private:
		wxString m_title;
};

class FbSearchFunction: public wxSQLite3ScalarFunction
{
	public:
		FbSearchFunction(const wxString & input);
	protected:
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		wxString Lower(const wxString & text);
		wxArrayString m_strings;
};

FbSearchFunction::FbSearchFunction(const wxString & input)
{
	wxString str = Lower(input);
	int i = wxNOT_FOUND;
	do {
		str = str.Trim(false);
		i = str.find(wxT(' '));
		if (i == wxNOT_FOUND) break;
		m_strings.Add( str.Left(i) );
		str = str.Mid(i);
	} while (true);
	str = str.Trim(true);
	if (!str.IsEmpty()) m_strings.Add(str);

	wxString log = wxT("Search template: ");
	size_t count = m_strings.Count();
	for (size_t i=0; i<count; i++) {
		log += wxString::Format(wxT("<%s> "), m_strings[i].c_str());
	}
	wxLogInfo(log);
}

wxString FbSearchFunction::Lower(const wxString & input)
{
	wxString output = input;
#if defined(__WIN32__)
	int len = output.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, output.c_str());
	CharLower(buf);
	output = buf;
	delete [] buf;
#else
	output.MakeLower();
#endif
	return output;
}

void FbSearchFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount != 1) {
		ctx.SetResultError(wxString::Format(_("SEARCH called with wrong number of arguments: %d."), argCount));
		return;
	}
	wxString text = Lower(ctx.GetString(0));

	size_t count = m_strings.Count();
	for (size_t i=0; i<count; i++) {
		if ( text.Find(m_strings[i]) == wxNOT_FOUND ) {
			ctx.SetResult(false);
			return;
		}
	}
	ctx.SetResult(true);
}

void * FrameSearchThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	EmptyBooks();

	wxString condition = wxT("SEARCH(books.title)");
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		database.AttachConfig();
		FbSearchFunction search(m_title);
		database.CreateFunction(wxT("SEARCH"), 1, search);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (result.Eof()) {
			wxCommandEvent event(fbEVT_BOOK_ACTION, ID_FOUND_NOTHING);
			wxPostEvent(m_frame, event);
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

void FbFrameSearch::OnFoundNothing(wxCommandEvent& event)
{
	wxString msg = wxString::Format(_("Ничего не найдено по шаблону «%s»"), m_title.c_str());
	wxMessageBox(msg, wxT("Поиск"));
	Close();
}
