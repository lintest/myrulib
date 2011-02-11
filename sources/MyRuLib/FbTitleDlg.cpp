#include <wx/artprov.h>
#include "FbTitleDlg.h"
#include "FbTreeView.h"
#include "FbAuthList.h"
#include "res/add.xpm"
#include "res/del.xpm"

//-----------------------------------------------------------------------------
//  FbTreeComboPopup
//-----------------------------------------------------------------------------

class FbTreeComboPopup : public FbTreeViewCtrl, public wxComboPopup
{
	public:
		virtual void Init()
			{ m_value = 0; }

		virtual bool Create( wxWindow* parent )
			{ return FbTreeViewCtrl::Create(parent, wxID_ANY, wxPoint(0, 0), wxDefaultSize, fbTR_NO_HEADER); }

		virtual wxWindow * GetControl()
			{ return this; }

		virtual void SetStringValue( const wxString& s )
		{
			/*
			int n = wxListView::FindItem(-1,s);
			if ( n >= 0 && n < GetItemCount() )
				wxListView::Select(n);
			*/
		}

		virtual wxString GetStringValue() const
		{
			return GetCurrentText();
		}

		void OnMouseMove(wxMouseEvent& event)
		{
			const wxPoint position = event.GetPosition();
			bool ok = FindAt(event.GetPosition(), true);
//			if (ok) m_value = GetCurrent();
			event.Skip();
		}

		// On mouse left, set the value and close the popup
		void OnMouseClick(wxMouseEvent& WXUNUSED(event))
		{
			m_value = m_itemHere;
			// TODO: Send event
			Dismiss();
		}

	    virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
		{
			FbModel * model = GetModel();
			if (model == NULL) return wxSize(minWidth, 0);
			size_t count = model->GetRowCount();
			if (count == 0) return wxSize(minWidth, 0);

			int border = wxSystemSettings::GetMetric(wxSYS_BORDER_Y) * 2 + 2;

			int h = GetRowHeight() * count + border;
			if (h > maxHeight) {
				count = (prefHeight - border) / GetRowHeight();
				h = GetRowHeight() * count + border;
			}
			return wxSize(minWidth, h);
		}

	protected:
		int m_value; // current item index
		int m_itemHere; // hot item in popup

	private:
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbTreeComboPopup);
};

IMPLEMENT_CLASS( FbTreeComboPopup, FbTreeViewCtrl )

BEGIN_EVENT_TABLE( FbTreeComboPopup, FbTreeViewCtrl )
    EVT_MOTION(FbTreeComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxListCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_UP(FbTreeComboPopup::OnMouseClick)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------
//  FbTitleDlg::SubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::SubPanel, wxPanel )

FbTitleDlg::SubPanel::SubPanel( wxWindow* parent, wxBoxSizer * owner )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL ), m_owner(owner)
{
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::AuthSubPanel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbTitleDlg::AuthSubPanel, FbTitleDlg::SubPanel )

FbTitleDlg::AuthSubPanel::AuthSubPanel( wxWindow* parent, wxBoxSizer * owner)
	: SubPanel( parent, owner )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	m_text.AssignModel(CreateModel());

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

	m_text.Connect( wxEVT_CHAR, wxKeyEventHandler( AuthSubPanel::OnChar ), NULL, this );
	m_text.Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AuthSubPanel::OnText ), NULL, this );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

FbTitleDlg::AuthSubPanel::~AuthSubPanel()
{
	m_text.Disconnect( wxEVT_CHAR, wxKeyEventHandler( AuthSubPanel::OnChar ), NULL, this );
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
		FbCollection::AddAuth(new FbCacheData(result));
		items.Add(code);
	}
	return new FbAuthListModel(items);
}

void FbTitleDlg::AuthSubPanel::OnChar( wxKeyEvent& event )
{
	event.Skip();
	if (m_text.IsPopupShown()) {
		FbTreeComboPopup * popup = wxDynamicCast(m_text.GetPopupControl(), FbTreeComboPopup);
		if (popup == NULL) return;
		FbModel * model = popup->GetModel();
		if (model == NULL) return;
		switch (event.GetKeyCode()) {
			case WXK_UP:
				model->GoPriorRow(); break;
			case WXK_DOWN:
				model->GoNextRow(); break;
		}
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

FbTitleDlg::SeqnSubPanel::SeqnSubPanel( wxWindow* parent, wxBoxSizer * owner)
	: SubPanel( parent, owner )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );

	m_text.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	m_text.SetMinSize( wxSize( 200, -1 ) );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("#"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_numb.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( &m_numb, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();

	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

    FbTreeComboPopup * popup = new FbTreeComboPopup();
    m_text.SetPopupControl(popup);

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

FbTitleDlg::TitlePanel::TitlePanel( wxWindow* parent)
	: wxScrolledWindow( parent )
{
	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;

	info = new wxStaticText( this, wxID_ANY, _("Book title"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_title.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizerMain->Add( &m_title, 0, wxALL|wxEXPAND, 3 );
	m_title.SetMinSize( wxSize( 300, -1 ) );

	info = new wxStaticText( this, wxID_ANY, _("Authors"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_authors = new wxBoxSizer(wxVERTICAL);
	m_authors->Add( new AuthSubPanel(this, m_authors), 1, wxEXPAND, 5 );
	m_authors->Add( new AuthSubPanel(this, m_authors), 1, wxEXPAND, 5 );
	m_authors->Add( new AuthSubPanel(this, m_authors), 1, wxEXPAND, 5 );
	fgSizerMain->Add( m_authors, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Series"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_series = new wxBoxSizer(wxVERTICAL);
	m_series->Add( new SeqnSubPanel(this, m_series), 1, wxEXPAND, 5 );
	m_series->Add( new SeqnSubPanel(this, m_series), 1, wxEXPAND, 5 );
	fgSizerMain->Add( m_series, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Genres"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_genres = new wxBoxSizer(wxVERTICAL);
	m_genres->Add( new AuthSubPanel(this, m_genres), 1, wxEXPAND, 5 );
	m_genres->Add( new AuthSubPanel(this, m_genres), 1, wxEXPAND, 5 );
	fgSizerMain->Add( m_genres, 1, wxEXPAND | wxRIGHT, 5 );

	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

void FbTitleDlg::TitlePanel::ArrangeControls()
{
	wxSize size = GetBestSize();
	size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	SetSize(size);

	wxWindow * notebook = GetParent();
	if (notebook) {
		FbTitleDlg * dialog = wxDynamicCast(notebook->GetParent(), FbTitleDlg);
		if (dialog) dialog->ArrangeControls();
	}
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
		}
	}

	ArrangeControls();
	GetSizer()->Fit(this);
}

//-----------------------------------------------------------------------------
//  FbTitleDlg
//-----------------------------------------------------------------------------

bool FbTitleDlg::Execute(int book)
{
	FbTitleDlg dlg(NULL, wxID_ANY, _("Properties"));
	dlg.Init();
	return dlg.ShowModal() == wxID_OK;
}

FbTitleDlg::FbTitleDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	sizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	wxPanel * panel = new TitlePanel( m_notebook );
	m_notebook->AddPage( panel, _("Title"), false );

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
		if (window) window->SetScrollbars(0, 0, 0, 0);
	}

	wxSize size = GetBestSize();
	size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	SetSize(size);

	for (size_t i = 0; i < count; i++) {
		wxScrolledWindow * window = wxDynamicCast(m_notebook->GetPage(i), wxScrolledWindow);
		if (window) window->SetScrollbars(20, 20, 50, 50);
	}
}
