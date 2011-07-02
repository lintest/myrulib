#include <wx/artprov.h>
#include "FbTitleDlg.h"
#include "controls/FbTreeView.h"
#include "models/FbAuthList.h"
#include "FbGenres.h"
#include "res/add.xpm"
#include "res/del.xpm"

//-----------------------------------------------------------------------------
//  FbTitleDlg::SubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::SubPanel, wxPanel )

FbTitleDlg::SubPanel::SubPanel( wxWindow* parent, wxBoxSizer * owner )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 ), m_owner(owner)
{
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::AuthSubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::AuthSubPanel, FbTitleDlg::SubPanel )

FbTitleDlg::AuthSubPanel::AuthSubPanel( wxWindow* parent, wxBoxSizer * owner, int book, const wxString & text )
	: SubPanel( parent, owner )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTAB_TRAVERSAL );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	m_text.AssignModel(CreateModel());

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

//	m_text.Connect( wxEVT_CHAR, wxKeyEventHandler( AuthSubPanel::OnChar ), NULL, this );
	m_text.Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AuthSubPanel::OnText ), NULL, this );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

FbTitleDlg::AuthSubPanel::~AuthSubPanel()
{
//	m_text.Disconnect( wxEVT_CHAR, wxKeyEventHandler( AuthSubPanel::OnChar ), NULL, this );
	m_text.Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AuthSubPanel::OnText ), NULL, this );
}

FbListModel * FbTitleDlg::AuthSubPanel::CreateModel()
{
	FbCommonDatabase database;
	wxString sql = wxT("SELECT docid, full_name, number FROM fts_auth INNER JOIN authors ON id=docid WHERE fts_auth MATCH ? ORDER BY full_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, wxT("a*"));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	wxArrayInt items;
	while (result.NextRow()) {
		int code = result.GetInt(0);
		FbCollection::AddAuth(code, result.GetString(1));
		items.Add(code);
	}
	return new FbAuthListModel(items);
}

void FbTitleDlg::AuthSubPanel::OnChar( wxKeyEvent& event )
{
	event.Skip();
	if (m_text.IsPopupShown()) {
		wxLogWarning(wxT("key"));
	} else {
		switch (event.GetKeyCode()) {
			case WXK_DOWN:
				m_text.ShowPopup(); break;
		}
	}
}

void FbTitleDlg::AuthSubPanel::OnText( wxCommandEvent& event )
{
	wxLogWarning(m_text.GetValue());
	event.Skip();
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::SeqnSubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::SeqnSubPanel, FbTitleDlg::SubPanel )

FbTitleDlg::SeqnSubPanel::SeqnSubPanel( wxWindow* parent, wxBoxSizer * owner, int code, const wxString & text, int numb )
	: SubPanel( parent, owner )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTAB_TRAVERSAL );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	m_text.SetMinSize( wxSize( 200, -1 ) );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("#"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString number;
	if (numb) number = wxString::Format(wxT("%d"), numb);
	m_numb.Create( this, wxID_ANY, number, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxTE_RIGHT );
	bSizerMain->Add( &m_numb, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

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
//  FbTitleDlg::GenrSubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::GenrSubPanel, FbTitleDlg::SubPanel )

FbTitleDlg::GenrSubPanel::GenrSubPanel( wxWindow* parent, wxBoxSizer * owner, const wxString & code, const wxString & text)
	: SubPanel( parent, owner )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTAB_TRAVERSAL );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

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

BEGIN_EVENT_TABLE( FbTitleDlg::TitlePanel, wxPanel )
	EVT_TOOL( wxID_ADD, FbTitleDlg::TitlePanel::OnToolAdd )
	EVT_TOOL( wxID_DELETE, FbTitleDlg::TitlePanel::OnToolDel )
END_EVENT_TABLE()

FbTitleDlg::TitlePanel::TitlePanel( wxWindow* parent, int book)
	: wxScrolledWindow( parent )
{
	FbCommonDatabase database;

	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 0, 0 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;

	info = new wxStaticText( this, wxID_ANY, _("Book title"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	wxString title;
	{
		wxSQLite3Statement stmt = database.PrepareStatement(wxT("SELECT title FROM books WHERE id=?"));
		stmt.Bind(1, book);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) title = result.GetString(0);
	}

	m_title.Create( this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizerMain->Add( &m_title, 0, wxALL|wxEXPAND, 3 );
	m_title.SetMinSize( wxSize( 300, -1 ) );

	info = new wxStaticText( this, wxID_ANY, _("Authors"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_authors = new wxBoxSizer(wxVERTICAL);
	{
		wxString sql = wxT("SELECT id, full_name FROM authors WHERE id IN(SELECT id_author FROM books WHERE id=? AND id_author<>0) ORDER BY search_name");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, book);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.Eof()) {
			m_authors->Add( new AuthSubPanel(this, m_authors), 1, wxEXPAND, 5 );
		} else {
			while (result.NextRow()) {
				SubPanel * panel = new AuthSubPanel(this, m_authors, result.GetInt(0), result.GetString(1));
				m_authors->Add( panel, 1, wxEXPAND, 5 );
			}
		}
	}
	fgSizerMain->Add( m_authors, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Series"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_series = new wxBoxSizer(wxVERTICAL);
	{
		wxString sql = wxT("SELECT b.id_seq, s.value, b.number FROM bookseq AS b INNER JOIN sequences AS s ON b.id_seq=s.id WHERE id_book=? ORDER BY value");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, book);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.Eof()) {
			m_series->Add( new SeqnSubPanel(this, m_series), 1, wxEXPAND, 5 );
		} else {
			while (result.NextRow()) {
				SubPanel * panel = new SeqnSubPanel(this, m_series, result.GetInt(0), result.GetString(1), result.GetInt(2));
				m_series->Add( panel, 1, wxEXPAND, 5 );
			}
		}
	}
	fgSizerMain->Add( m_series, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Genres"), wxDefaultPosition, wxDefaultSize, 0 );
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
			m_genres->Add( new GenrSubPanel(this, m_genres), 1, wxEXPAND, 5 );
		} else {
			while (result.NextRow()) {
				SubPanel * panel = new GenrSubPanel(this, m_genres, result.GetString(0), result.GetString(1));
				m_genres->Add( panel, 1, wxEXPAND, 5 );
			}
		}
	}
	fgSizerMain->Add( m_genres, 1, wxEXPAND | wxRIGHT, 5 );

	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

void FbTitleDlg::TitlePanel::ArrangeControls()
{
	wxWindow * notebook = GetParent();
	if (notebook == NULL) return;

	FbTitleDlg * dialog = wxDynamicCast(notebook->GetParent(), FbTitleDlg);
	if (dialog == NULL) return;

	dialog->ArrangeControls();
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

	ArrangeControls();
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
			owner->Detach(panel);
			panel->Destroy();
			ArrangeControls();
		}
	}
}

//-----------------------------------------------------------------------------
//  FbTitleDlg
//-----------------------------------------------------------------------------

bool FbTitleDlg::Execute(int book)
{
	FbTitleDlg dlg(NULL, book);
	dlg.Init();
	return dlg.ShowModal() == wxID_OK;
}

FbTitleDlg::FbTitleDlg( wxWindow* parent, int book )
	: FbDialog( parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER | wxTAB_TRAVERSAL )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	sizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	wxPanel * panel = new TitlePanel( m_notebook, book );
	m_notebook->AddPage( panel, _("General"), false );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizer->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	this->SetSizer( sizer );
	this->Layout();
	this->Centre( wxBOTH );
}

FbTitleDlg::~FbTitleDlg()
{
}

void FbTitleDlg::Init()
{
	ArrangeControls();
	size_t count = m_notebook->GetPageCount();
	for (size_t i = 0; i < count; i++) {
		wxScrolledWindow * window = wxDynamicCast(m_notebook->GetPage(i), wxScrolledWindow);
		if (window) window->SetScrollbars(20, 20, 50, 50);
	}
}

void FbTitleDlg::ArrangeControls()
{
	size_t count = m_notebook->GetPageCount();
	for (size_t i = 0; i < count; i++) {
		wxScrolledWindow * window = wxDynamicCast(m_notebook->GetPage(i), wxScrolledWindow);
		if (window) {
			window->Layout();
			wxSize size = window->GetSizer()->ComputeFittingClientSize(window);
			size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
			window->SetScrollbars(0, 0, 0, 0);
			window->SetMinSize(size);
			window->SetSize(size);
		}
	}

	wxSize size = GetBestSize();
	size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	SetSize(size);

	for (size_t i = 0; i < count; i++) {
		wxScrolledWindow * window = wxDynamicCast(m_notebook->GetPage(i), wxScrolledWindow);
		if (window) {
			window->SetScrollbars(20, 20, 50, 50);
			window->SetMinSize(wxSize(-1, -1));
			window->Layout();
		}
	}

	Layout();
}
