#include <wx/artprov.h>
#include "FbTitleDlg.h"
#include "FbString.h"
#include "FbConst.h"
#include "FbDateTime.h"
#include "controls/FbTreeView.h"
#include "models/FbAuthList.h"
#include "models/FbSeqnList.h"
#include "FbGenres.h"
#include "res/add.xpm"
#include "res/del.xpm"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbTitleDlg::SubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::SubPanel, wxPanel )

FbTitleDlg::SubPanel::SubPanel( wxWindow* parent, wxBoxSizer * owner )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL ), m_owner(owner)
{
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::AuthPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::AuthPanel, FbTitleDlg::SubPanel )

BEGIN_EVENT_TABLE(FbTitleDlg::AuthPanel, FbTitleDlg::SubPanel)
	EVT_TEXT_ENTER(ID_MASTER_FIND, FbTitleDlg::AuthPanel::OnTextEnter)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbTitleDlg::AuthPanel::OnModel)
	EVT_TIMER(ID_MASTER_LIST, FbTitleDlg::AuthPanel::OnTimer)
END_EVENT_TABLE()

FbTitleDlg::AuthPanel::AuthPanel( wxWindow* parent, wxBoxSizer * owner, int code, const wxString & text )
	: SubPanel( parent, owner )
	, m_timer(this, ID_MASTER_LIST)
	, m_thread(NULL)
	, m_code(code)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, ID_MASTER_FIND, text.Strip(), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

	m_text.Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AuthPanel::OnText ), NULL, this );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

FbTitleDlg::AuthPanel::~AuthPanel()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
	m_text.Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AuthPanel::OnText ), NULL, this );
}

void FbTitleDlg::AuthPanel::OnTextEnter( wxCommandEvent& event )
{
	m_timer.Stop();
	StartThread();
}

void FbTitleDlg::AuthPanel::OnText( wxCommandEvent& event )
{
	m_timer.Start(1000, true);
}

void FbTitleDlg::AuthPanel::OnTimer( wxTimerEvent& event )
{
	StartThread();
}

void FbTitleDlg::AuthPanel::StartThread()
{
	wxString text = m_text.GetValue();
	if (text.IsEmpty() || wxString(m_text.GetCurrent()) == text) return;

	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}

	m_thread = new SearchThread(this, wxT("fts_auth"), text);
	m_thread->Execute();
}

void FbTitleDlg::AuthPanel::OnModel( FbArrayEvent& event )
{
	FbAuthListModel * model = new FbAuthListModel(event.GetArray());
	m_text.AssignModel(model);
	switch (size_t count = model->GetRowCount()) {
		case 0: {
			m_code = 0; 
		} break;
		case 1: {
			FbModelItem item = m_text.GetCurrent();
			FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
			if (current) m_code = current->GetCode();
			m_text.SetValue(item);
		} break;
		default: {
			m_text.ShowPopup(); 
		} break;
	}
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::SeqnPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::SeqnPanel, FbTitleDlg::SubPanel )

BEGIN_EVENT_TABLE(FbTitleDlg::SeqnPanel, FbTitleDlg::SubPanel)
	EVT_TEXT_ENTER(ID_MASTER_FIND, FbTitleDlg::SeqnPanel::OnTextEnter)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbTitleDlg::SeqnPanel::OnModel)
	EVT_TIMER(ID_MASTER_LIST, FbTitleDlg::SeqnPanel::OnTimer)
END_EVENT_TABLE()

FbTitleDlg::SeqnPanel::SeqnPanel( wxWindow* parent, wxBoxSizer * owner, int code, const wxString & text, int numb )
	: SubPanel( parent, owner )
	, m_timer(this, ID_MASTER_LIST)
	, m_thread(NULL)
	, m_code(code)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, ID_MASTER_FIND, text.Strip(), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	m_text.SetMinSize( wxSize( 200, -1 ) );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("#") );
	bSizerMain->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString number;
	if (numb) number = wxString::Format(wxT("%d"), numb);
	m_numb.Create( this, wxID_ANY, number, wxDefaultPosition, wxDefaultSize, wxTE_RIGHT | wxTE_PROCESS_ENTER );
	bSizerMain->Add( &m_numb, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

	m_text.Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SeqnPanel::OnText ), NULL, this );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

FbTitleDlg::SeqnPanel::~SeqnPanel()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
	m_text.Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SeqnPanel::OnText ), NULL, this );
}

void FbTitleDlg::SeqnPanel::OnTextEnter( wxCommandEvent& event )
{
	m_timer.Stop();
	StartThread();
}

void FbTitleDlg::SeqnPanel::OnText( wxCommandEvent& event )
{
	m_timer.Start(1000, true);
}

void FbTitleDlg::SeqnPanel::OnTimer( wxTimerEvent& event )
{
	StartThread();
}

void FbTitleDlg::SeqnPanel::StartThread()
{
	wxString text = m_text.GetValue();
	if (text.IsEmpty() || wxString(m_text.GetCurrent()) == text) return;

	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}

	m_thread = new SearchThread(this, wxT("fts_seqn"), text);
	m_thread->Execute();
}

void FbTitleDlg::SeqnPanel::OnModel( FbArrayEvent& event )
{
	FbSeqnListModel * model = new FbSeqnListModel(event.GetArray());
	m_text.AssignModel(model);
	switch (size_t count = model->GetRowCount()) {
		case 0: {
			m_code = 0; 
		} break;
		case 1: {
			FbModelItem item = m_text.GetCurrent();
			FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
			if (current) m_code = current->GetCode();
			m_text.SetValue(item);
		} break;
		default: {
			m_text.ShowPopup(); 
		} break;
	}
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::GenrPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::GenrPanel, FbTitleDlg::SubPanel )

FbTitleDlg::GenrPanel::GenrPanel( wxWindow* parent, wxBoxSizer * owner, const wxString & code, const wxString & text)
	: SubPanel( parent, owner )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxCB_READONLY | wxTE_PROCESS_ENTER );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	m_text.AssignModel(FbGenres::CreateModel());

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::TitlePanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::TitlePanel, wxScrolledWindow )

BEGIN_EVENT_TABLE( FbTitleDlg::TitlePanel, wxScrolledWindow )
	EVT_TOOL( wxID_ADD, FbTitleDlg::TitlePanel::OnToolAdd )
	EVT_TOOL( wxID_DELETE, FbTitleDlg::TitlePanel::OnToolDel )
END_EVENT_TABLE()

FbTitleDlg::TitlePanel::TitlePanel( wxWindow* parent, int book, wxSQLite3Database &database, wxSQLite3ResultSet &result)
	: wxScrolledWindow( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxTAB_TRAVERSAL )
{
	wxFlexGridSizer * fgSizerMain = new wxFlexGridSizer( 2 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;

	info = new wxStaticText( this, wxID_ANY, _("Book title") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_title.Create( this, wxID_ANY, result.GetString(0), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	fgSizerMain->Add( &m_title, 0, wxALL|wxEXPAND, 3 );
	m_title.SetMinSize( wxSize( 300, -1 ) );

	info = new wxStaticText( this, wxID_ANY, _("Authors") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_authors = new wxBoxSizer(wxVERTICAL);
	{
		wxString sql = fbT("SELECT id, full_name FROM authors WHERE id IN(SELECT id_author FROM books WHERE id=? AND id_author<>0) ORDER BY 2") << fbCOLLATE_CYR;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, book);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.Eof()) {
			m_authors->Add( new AuthPanel(this, m_authors), 1, wxEXPAND, 5 );
		} else {
			while (result.NextRow()) {
				SubPanel * panel = new AuthPanel(this, m_authors, result.GetInt(0), result.GetString(1));
				m_authors->Add( panel, 1, wxEXPAND, 5 );
			}
		}
	}
	fgSizerMain->Add( m_authors, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Series") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_series = new wxBoxSizer(wxVERTICAL);
	{
		wxString sql = wxT("SELECT b.id_seq, s.value, b.number FROM bookseq AS b INNER JOIN sequences AS s ON b.id_seq=s.id WHERE id_book=? ORDER BY value");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, book);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.Eof()) {
			m_series->Add( new SeqnPanel(this, m_series), 1, wxEXPAND, 5 );
		} else {
			while (result.NextRow()) {
				SubPanel * panel = new SeqnPanel(this, m_series, result.GetInt(0), result.GetString(1), result.GetInt(2));
				m_series->Add( panel, 1, wxEXPAND, 5 );
			}
		}
	}
	fgSizerMain->Add( m_series, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Genres") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_genres = new wxBoxSizer(wxVERTICAL);
	{
		FbGenreFunction func_genre;
		database.CreateFunction(wxT("GENRE"), 1, func_genre);
		wxString sql = wxT("SELECT id_genre, GENRE(id_genre) FROM genres WHERE id_book=? ORDER BY 2");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, book);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.Eof()) {
			m_genres->Add( new GenrPanel(this, m_genres), 1, wxEXPAND, 5 );
		} else {
			while (result.NextRow()) {
				SubPanel * panel = new GenrPanel(this, m_genres, result.GetString(0), result.GetString(1));
				m_genres->Add( panel, 1, wxEXPAND, 5 );
			}
		}
	}
	fgSizerMain->Add( m_genres, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Language") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	wxBoxSizer * bSizerFile = new wxBoxSizer( wxHORIZONTAL );

	m_lang.Create( this, wxID_ANY, result.GetString(1), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_lang, 1, wxALL, 3 );

	info = new wxStaticText( this, wxID_ANY, _("Extension") );
	info->Wrap( -1 );
	bSizerFile->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	m_type.Create( this, wxID_ANY, result.GetString(2), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_type, 1, wxALL, 3 );

	info = new wxStaticText( this, wxID_ANY, _("Date") );
	info->Wrap( -1 );
	bSizerFile->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	FbDateTime date = result.GetInt(3);
	m_date.Create( this, wxID_ANY, date, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_date, 1, wxALL, 3 );

	fgSizerMain->Add( bSizerFile, 0, wxEXPAND, 0 );

	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

void FbTitleDlg::TitlePanel::ArrangeControls(int height)
{
	wxWindow * notebook = GetParent();
	if (notebook == NULL) return;

	FbTitleDlg * dialog = wxDynamicCast(notebook->GetParent(), FbTitleDlg);
	if (dialog == NULL) return;

	dialog->ArrangeControls(height);
}

void FbTitleDlg::TitlePanel::OnToolAdd( wxCommandEvent& event )
{
	wxToolBar * toolbar = wxDynamicCast(event.GetEventObject(), wxToolBar);
	if (toolbar == NULL) return;

	SubPanel * panel = wxDynamicCast(toolbar->GetParent(), SubPanel);
	if (panel == NULL) return;

	wxBoxSizer * owner = panel->GetOwner();
	if (owner == NULL) return;

	wxWindow * prior = owner->GetChildren().GetLast()->GetData()->GetWindow();
	panel = panel->New(this, owner);
	panel->MoveAfterInTabOrder(prior);
	owner->Add( panel, 1, wxEXPAND, 5 );

	int height = panel->GetSize().y;
	ArrangeControls(height);
}

void FbTitleDlg::TitlePanel::OnToolDel( wxCommandEvent& event )
{
	wxToolBar * toolbar = wxDynamicCast(event.GetEventObject(), wxToolBar);
	if (toolbar == NULL) return;

	SubPanel * panel = wxDynamicCast(toolbar->GetParent(), SubPanel);
	if (panel == NULL) return;

	wxBoxSizer * owner = panel->GetOwner();
	if (owner == NULL) return;

	wxSizerItem * item = owner->GetItem(panel);
	if (item) {
		if (owner->GetChildren().GetCount() == 1) {
			panel->Empty();
		} else {
			int height = panel->GetSize().y;
			owner->Detach(panel);
			panel->Destroy();
			ArrangeControls(-height);
		}
	}
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::DescrPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::DescrPanel, wxPanel )

FbTitleDlg::DescrPanel::DescrPanel( wxWindow* parent, int book, wxSQLite3ResultSet &result )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_text.Create( this, wxID_ANY, result.GetString(4), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP );
	bSizerMain->Add( &m_text, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::SearchThread
//-----------------------------------------------------------------------------

void * FbTitleDlg::SearchThread::Entry()
{
	wxString text = m_text.Strip();
	MakeLower(text) << wxT('*');

	FbCommonDatabase database;
	wxString sql = wxT("SELECT docid FROM %s WHERE %s MATCH ? ORDER BY content");
	sql = wxString::Format(sql, m_table.c_str(), m_table.c_str());
	wxSQLite3Statement stmt = database.PrepareStatement(sql); stmt.Bind(1, text);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	wxArrayInt items;
	while (result.NextRow()) {
		if (IsClosed()) return NULL;
		items.Add(result.GetInt(0));
	}
	FbArrayEvent(ID_MODEL_CREATE, items).Post(m_frame);

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbTitleDlg
//-----------------------------------------------------------------------------

bool FbTitleDlg::Execute(int book)
{
	FbTitleDlg dlg(wxGetApp().GetTopWindow(), book);
	return dlg.ShowModal() == wxID_OK;
}

FbTitleDlg::FbTitleDlg( wxWindow* parent, int book )
	: FbDialog( parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
{
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	sizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	FbCommonDatabase database;
	wxString sql = fbT("SELECT title, lang, file_type, created, description FROM books WHERE id=? LIMIT 1");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, book);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	m_notebook->AddPage( m_title = new TitlePanel( m_notebook, book, database, result ), _("General"), true );
	m_notebook->AddPage( m_descr = new DescrPanel( m_notebook, book, result ), _("Annotation"), false );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizer->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	SetSizer( sizer );
	Layout();

	int h;
	parent->GetSize(NULL, &h);
	wxSize size = GetBestSize();
	size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	size.y += 2;
	if (size.y > h) size.y = h;
	SetSize(size);

	Centre( wxBOTH );

	size_t count = m_notebook->GetPageCount();
	for (size_t i = 0; i < count; i++) {
		wxScrolledWindow * window = wxDynamicCast(m_notebook->GetPage(i), wxScrolledWindow);
		if (window) {
			window->SetScrollbars(20, 20, 50, 50);
			window->Layout();
		}
	}
}

FbTitleDlg::~FbTitleDlg()
{
}

void FbTitleDlg::ArrangeControls(int height)
{
	wxSize size = GetSize();
	size.y += height;
	SetSize(size);
	Layout();
}
