#include <wx/artprov.h>
#include "FbTitleDlg.h"
#include "FbString.h"
#include "FbConst.h"
#include "FbDateTime.h"
#include "controls/FbTreeView.h"
#include "models/FbAuthList.h"
#include "models/FbGenrTree.h"
#include "models/FbSeqnList.h"
#include "FbGenres.h"
#include "res/add.xpm"
#include "res/del.xpm"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbTitleDlg::SubPanel
//-----------------------------------------------------------------------------

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
	m_text.Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AuthPanel::OnText ), NULL, this );
	FbThread::DeleteRef(m_thread);
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
	if (text.IsEmpty() || m_text.GetText() == text) return;
	FbThread::DeleteRef(m_thread);
	m_thread = new SearchThread(this, ID_MODEL_CREATE, wxT("fts_auth"), text);
	m_thread->Execute();
}

void FbTitleDlg::AuthPanel::OnModel( FbArrayEvent& event )
{
	int m_code = event.GetInt();
	FbAuthListModel * model = new FbAuthListModel(event.GetArray(), m_code);
	m_text.AssignModel(model);
	if (m_code) {
		FbModelItem item = m_text.GetCurrent();
		FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
		if (current) m_code = current->GetCode();
		m_text.SetValue(item);
	} else {
		if (model->GetRowCount()) m_text.ShowPopup();
	}
}

int FbTitleDlg::AuthPanel::GetCode()
{
	FbModelItem item = m_text.GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	return current ? current->GetCode() : m_code;
}

void FbTitleDlg::AuthPanel::Empty()
{
	m_timer.Stop();
	FbThread::DeleteRef(m_thread);
	m_text.AssignModel(NULL);
	m_text.SetValue(wxEmptyString);
    m_code = 0;
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
	m_text.Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SeqnPanel::OnText ), NULL, this );
	FbThread::DeleteRef(m_thread);
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
	if (text.IsEmpty() || m_text.GetText() == text) return;
	FbThread::DeleteRef(m_thread);
	m_thread = new SearchThread(this, ID_MODEL_CREATE, wxT("fts_seqn"), text);
	m_thread->Execute();
}

void FbTitleDlg::SeqnPanel::OnModel( FbArrayEvent& event )
{
	int m_code = event.GetInt();
	FbSeqnListModel * model = new FbSeqnListModel(event.GetArray(), m_code);
	m_text.AssignModel(model);
	if (m_code) {
		FbModelItem item = m_text.GetCurrent();
		FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
		if (current) m_code = current->GetCode();
		m_text.SetValue(item);
	} else {
		if (model->GetRowCount()) m_text.ShowPopup();
	}
}

int FbTitleDlg::SeqnPanel::GetCode()
{
	FbModelItem item = m_text.GetCurrent();
	FbSeqnListData * current = wxDynamicCast(&item, FbSeqnListData);
	return current ? current->GetCode() : m_code;
}

int FbTitleDlg::SeqnPanel::GetNumb()
{
	long value;
	return m_numb.GetValue().ToLong(&value) ? value : 0;
}

void FbTitleDlg::SeqnPanel::Empty()
{
	m_timer.Stop();
	FbThread::DeleteRef(m_thread);
	m_text.AssignModel(NULL);
	m_text.SetValue(wxEmptyString);
	m_numb.SetValue(wxEmptyString);
    m_code = 0;
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
	m_text.AssignModel(FbGenres::CreateModel(code));

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

wxString FbTitleDlg::GenrPanel::GetCode()
{
	FbModelItem item = m_text.GetCurrent();
	FbGenrChildData * child = wxDynamicCast(&item, FbGenrChildData);
	if (child) return child->GetCode();
	FbGenrParentData * parent = wxDynamicCast(&item, FbGenrParentData);
	if (parent) return parent->GetCode();
	return wxEmptyString;
}

void FbTitleDlg::GenrPanel::Empty()
{
	m_text.AssignModel(FbGenres::CreateModel());
	m_text.SetValue(_("No genre"));
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::AuthSizer
//-----------------------------------------------------------------------------

FbTitleDlg::AuthSizer::AuthSizer(wxWindow* parent, wxSQLite3Database &database, const wxString &ids)
	: wxBoxSizer(wxVERTICAL)
{
	wxString sql = wxString::Format(wxT("SELECT id, full_name FROM authors WHERE id IN(SELECT DISTINCT id_author FROM books WHERE id IN(%s) AND id_author<>0) ORDER BY 2"), ids.c_str()) << fbCOLLATE_CYR;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (result.Eof()) {
		Add( new AuthPanel(parent, this), 1, wxEXPAND, 5 );
	} else {
		while (result.NextRow()) {
			SubPanel * panel = new AuthPanel(parent, this, result.GetInt(0), result.GetString(1));
			Add( panel, 1, wxEXPAND, 5 );
		}
	}
}

void FbTitleDlg::AuthSizer::Get(wxArrayInt &code, wxString &text)
{
	wxSizerItemList & list = GetChildren();
	wxSizerItemList::iterator it;
	for (it = list.begin(); it != list.end(); it++) {
		if (AuthPanel * panel = wxDynamicCast((*it)->GetWindow(), AuthPanel)) {
			int id = panel->GetCode();
			if (id && code.Index(id) == wxNOT_FOUND) {
				if (!text.IsEmpty()) text << wxT(',');
				text << id;
				code.Add(id);
			}
		}
	}
	if (code.Count()) return;
	text = wxT('0');
	code.Add(0);
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::SeqnSizer
//-----------------------------------------------------------------------------

FbTitleDlg::SeqnSizer::SeqnSizer(wxWindow* parent, wxSQLite3Database &database, const wxString &sql)
	: wxBoxSizer(wxVERTICAL)
{
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (result.Eof()) {
		Add( new SeqnPanel(parent, this), 1, wxEXPAND, 5 );
	} else {
		while (result.NextRow()) {
			SubPanel * panel = new SeqnPanel(parent, this, result.GetInt(0), result.GetString(1), result.GetInt(2));
			Add( panel, 1, wxEXPAND, 5 );
		}
	}
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::GenrSizer
//-----------------------------------------------------------------------------

FbTitleDlg::GenrSizer::GenrSizer(wxWindow* parent, wxSQLite3Database &database, const wxString &sql)
	: wxBoxSizer(wxVERTICAL)
{
	FbGenreFunction func_genre;
	database.CreateFunction(wxT("GENRE"), 1, func_genre);
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (result.Eof()) {
		Add( new GenrPanel(parent, this), 1, wxEXPAND, 5 );
	} else {
		while (result.NextRow()) {
			SubPanel * panel = new GenrPanel(parent, this, result.GetString(0), result.GetString(1));
			Add( panel, 1, wxEXPAND, 5 );
		}
	}
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::TitlePanel
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbTitleDlg::TitlePanel, wxScrolledWindow )
	EVT_TOOL( wxID_ADD, FbTitleDlg::TitlePanel::OnToolAdd )
	EVT_TOOL( wxID_DELETE, FbTitleDlg::TitlePanel::OnToolDel )
END_EVENT_TABLE()

FbTitleDlg::TitlePanel::TitlePanel( wxWindow* parent)
	: wxScrolledWindow( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxTAB_TRAVERSAL )
{
}

void FbTitleDlg::TitlePanel::ArrangeControls(int height)
{
	wxWindow * parent = GetParent();
	if (!parent) return;

	FbTitleDlg * dialog = wxDynamicCast(parent, FbTitleDlg);
	if (!dialog) dialog = wxDynamicCast(parent->GetParent(), FbTitleDlg);
	if (!dialog) return;

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

void FbTitleDlg::TitlePanel::GetAuths(wxArrayInt &code, wxString &text)
{
	m_authors->Get(code, text);
}

void FbTitleDlg::TitlePanel::GetData(BookData & data)
{
	data.title = m_title.GetValue();
	data.genr = GetGenr();
	data.lang = m_lang.GetValue();
	data.type = m_type.GetValue();
	data.date =  FbDateTime(m_date.GetValue()).Code();
}

wxString FbTitleDlg::TitlePanel::GetGenr()
{
	wxString text;
	wxArrayString items;
	wxSizerItemList & list = m_genres->GetChildren();
	wxSizerItemList::iterator it;
	for (it = list.begin(); it != list.end(); it++) {
		if (GenrPanel * panel = wxDynamicCast((*it)->GetWindow(), GenrPanel)) {
			wxString code = panel->GetCode();
			if (items.Index(code) == wxNOT_FOUND) {
				items.Add(code);
				text << code;
			}
		}
	}
	return text;
}

void FbTitleDlg::TitlePanel::SaveGenr(int book, wxSQLite3Database &database)
{
	wxString sql = wxT("INSERT OR REPLACE INTO genres(id_book,id_genre) VALUES (?,?)");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);

	wxString text;
	wxArrayString items;
	wxSizerItemList & list = m_genres->GetChildren();
	wxSizerItemList::iterator it;
	for (it = list.begin(); it != list.end(); it++) {
		if (GenrPanel * panel = wxDynamicCast((*it)->GetWindow(), GenrPanel)) {
			wxString code = panel->GetCode();
			if (code.IsEmpty() || items.Index(code) != wxNOT_FOUND) continue;
			items.Add(code);
			if (!text.IsEmpty()) text << wxT(',');
			text << wxT('"') << code << wxT('"');
			stmt.Bind(1, book);
			stmt.Bind(2, code);
			stmt.ExecuteUpdate();
			stmt.Reset();
		}
	}

	sql = wxT("DELETE FROM genres WHERE id_book=%d AND NOT id_genre IN(%s)");
	sql = wxString::Format(sql, book, text.c_str());
	database.ExecuteUpdate(sql);
}

void FbTitleDlg::TitlePanel::SaveSeqn(int book, wxSQLite3Database &database)
{
	wxString sql = wxT("INSERT OR REPLACE INTO bookseq(id_book,id_seq,number) VALUES (?,?,?)");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);

	wxString text;
	wxArrayInt items;
	wxSizerItemList & list = m_series->GetChildren();
	wxSizerItemList::iterator it;
	for (it = list.begin(); it != list.end(); it++) {
		if (SeqnPanel * panel = wxDynamicCast((*it)->GetWindow(), SeqnPanel)) {
			int code = panel->GetCode();
			if (!code || items.Index(code) != wxNOT_FOUND) continue;
			items.Add(code);
			if (!text.IsEmpty()) text << wxT(',');
			text << code;
			stmt.Bind(1, book);
			stmt.Bind(2, code);
			stmt.Bind(3, panel->GetNumb());
			stmt.ExecuteUpdate();
			stmt.Reset();
		}
	}

	sql = wxT("DELETE FROM bookseq WHERE id_book=%d AND NOT id_seq IN(%s)");
	sql = wxString::Format(sql, book, text.c_str());
	database.ExecuteUpdate(sql);
}

//-----------------------------------------------------------------------------
//  FbTitleDlg::DscrPanel
//-----------------------------------------------------------------------------

FbTitleDlg::DscrPanel::DscrPanel( wxWindow* parent, int book, wxSQLite3ResultSet &result )
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_text.Create( this, wxID_ANY, result.GetString(wxT("description")), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP );
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

	int code = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (IsClosed()) return NULL;
		code = result.GetInt(0);
		items.Add(code);
	}

	switch (items.Count()) {
		case 1: {
			wxString text = m_text.BeforeFirst(wxT(' '));
			if (text == m_text) break;
			MakeLower(text) << wxT('*');
			items.Clear();
			stmt.Reset();
			stmt.Bind(1, text);
			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			while (result.NextRow()) {
				if (IsClosed()) return NULL;
				items.Add(result.GetInt(0));
			}
		} break;
		case 0:
		default:
			code = 0;
	}

	FbArrayEvent(m_id, items, code).Post(m_frame);

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbTitleDlg
//-----------------------------------------------------------------------------

FbTitleDlg::FbTitleDlg(const wxString &title)
	: FbDialog( wxGetApp().GetTopWindow(), wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
{
}

void FbTitleDlg::ArrangeControls(int height)
{
	wxSize size = GetSize();
	size.y += height;
	SetSize(size);
	Layout();
}

//-----------------------------------------------------------------------------
//  FbSingleTitleDlg::MainPanel
//-----------------------------------------------------------------------------

FbSingleTitleDlg::MainPanel::MainPanel(wxWindow* parent, int book, wxSQLite3Database &database, wxSQLite3ResultSet &result)
	: TitlePanel( parent )
{
	wxFlexGridSizer * fgSizerMain = new wxFlexGridSizer( 2 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;

	info = new wxStaticText( this, wxID_ANY, _("Book title") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_title.Create( this, wxID_ANY, result.GetString(wxT("title")), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	fgSizerMain->Add( &m_title, 0, wxALL|wxEXPAND, 3 );
	m_title.SetMinSize( wxSize( 300, -1 ) );

	info = new wxStaticText( this, wxID_ANY, _("Authors") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	wxString sql;

	wxString id; id << book;
	fgSizerMain->Add( m_authors = new AuthSizer(this, database, id), 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Series") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	sql = wxString::Format(wxT("SELECT b.id_seq, s.value, b.number FROM bookseq AS b INNER JOIN sequences AS s ON b.id_seq=s.id WHERE id_book=%d ORDER BY value"), book) << fbCOLLATE_CYR;
	fgSizerMain->Add( m_series = new SeqnSizer(this, database, sql), 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Genres") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	sql = wxString::Format(wxT("SELECT id_genre, GENRE(id_genre) FROM genres WHERE id_book=%d ORDER BY 2"), book) << fbCOLLATE_CYR;
	fgSizerMain->Add( m_genres = new GenrSizer(this, database, sql), 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Language") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	wxBoxSizer * bSizerFile = new wxBoxSizer( wxHORIZONTAL );

	m_lang.Create( this, wxID_ANY, result.GetString(wxT("lang")), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_lang, 1, wxALL, 3 );

	info = new wxStaticText( this, wxID_ANY, _("Extension") );
	info->Wrap( -1 );
	bSizerFile->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	m_type.Create( this, wxID_ANY, result.GetString(wxT("file_type")), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_type, 1, wxALL, 3 );

	info = new wxStaticText( this, wxID_ANY, _("Date") );
	info->Wrap( -1 );
	bSizerFile->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	FbDateTime date = result.GetInt(wxT("created"));
	m_date.Create( this, wxID_ANY, date, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_date, 1, wxALL, 3 );

	fgSizerMain->Add( bSizerFile, 0, wxEXPAND, 0 );

	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

//-----------------------------------------------------------------------------
//  FbSingleTitleDlg
//-----------------------------------------------------------------------------

bool FbSingleTitleDlg::Execute(int book)
{
	FbCommonDatabase database;
	wxString sql = fbT("SELECT * FROM books WHERE id=? LIMIT 1");
	wxSQLite3Statement stmt = database.PrepareStatement(sql); stmt.Bind(1, book);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) return false;

	FbSingleTitleDlg dlg(book, database, result);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) dlg.Save(book, database, result);
	return ok;
}

FbSingleTitleDlg::FbSingleTitleDlg(int book, wxSQLite3Database &database, wxSQLite3ResultSet &result)
	: FbTitleDlg(_("Properties"))
{
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	sizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	m_notebook->AddPage( m_title = new MainPanel( m_notebook, book, database, result ), _("General"), true );
	m_notebook->AddPage( m_descr = new DscrPanel( m_notebook, book, result ), _("Annotation"), false );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizer->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	SetSizer( sizer );
	Layout();

	int h = GetParent()->GetSize().y;
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

void FbSingleTitleDlg::Save(int book, wxSQLite3Database &database, wxSQLite3ResultSet &result)
{
	BookData data;
	wxArrayInt ids; wxString authors;
	m_title->GetAuths(ids, authors);
	m_title->GetData(data);
	data.dscr = m_descr->GetValue();
	data.arch = result.GetInt(wxT("id_archive"));
	data.size = result.GetInt64(wxT("file_size"));
	data.file = result.GetString(wxT("file_name"));
	data.path = result.GetString(wxT("file_path"));
	data.md5s = result.GetString(wxT("md5sum"));

	FbAutoCommit commit(database);
	wxString sql = wxT("INSERT OR REPLACE INTO books(id,id_author,title,genres,id_archive,file_name,file_path,file_size,file_type,lang,description,created,md5sum) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);

	size_t count = ids.Count();
	for (size_t i = 0; i < count; i++) {
		stmt.Bind(1, book);
		stmt.Bind(2, ids[i]);
		stmt.Bind(3, data.title);
		stmt.Bind(4, data.genr);
		stmt.Bind(5, data.arch);
		stmt.Bind(6, data.file);
		stmt.Bind(7, data.path);
		stmt.Bind(8, data.size);
		stmt.Bind(9, data.type);
		stmt.Bind(10, data.lang);
		stmt.Bind(11, data.dscr);
		stmt.Bind(12, data.date);
		stmt.Bind(13, data.md5s);
		stmt.ExecuteUpdate();
		stmt.Reset();
	}

	{
		wxString sql = wxT("INSERT OR REPLACE INTO fts_book(content,docid) VALUES(LOW(?),?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, data.title);
		stmt.Bind(2, book);
		stmt.ExecuteUpdate();
	}

	sql = wxT("DELETE FROM books WHERE id=%d AND NOT id_author IN(%s)");
	sql = wxString::Format(sql, book, authors.c_str());
	database.ExecuteUpdate(sql);

	m_title->SaveSeqn(book, database);
	m_title->SaveGenr(book, database);

	FbCollection::ResetBook(book);
	FbCollection::ResetInfo(book);

	FbCommandEvent(fbEVT_BOOK_ACTION, ID_UPDATE_BOOK, book).Post();
}

//-----------------------------------------------------------------------------
//  FbGroupTitleDlg::MainPanel
//-----------------------------------------------------------------------------

FbGroupTitleDlg::MainPanel::MainPanel(wxWindow* parent, const wxArrayInt &items, const wxString &codes, wxSQLite3Database &database)
	: TitlePanel( parent )
{
	wxFlexGridSizer * fgSizerMain = new wxFlexGridSizer( 2 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;

	info = new wxStaticText( this, wxID_ANY, _("Authors") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	fgSizerMain->Add( m_authors = new AuthSizer(this, database, codes), 1, wxEXPAND | wxRIGHT, 5 );
/*
	wxString sql;

	info = new wxStaticText( this, wxID_ANY, _("Series") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	sql = wxString::Format(wxT("SELECT b.id_seq, s.value, b.number FROM bookseq AS b INNER JOIN sequences AS s ON b.id_seq=s.id WHERE id_book=%d ORDER BY value"), book) << fbCOLLATE_CYR;
	fgSizerMain->Add( m_series = new SeqnSizer(this, database, sql), 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Genres") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	sql = wxString::Format(wxT("SELECT id_genre, GENRE(id_genre) FROM genres WHERE id_book=%d ORDER BY 2"), book) << fbCOLLATE_CYR;
	fgSizerMain->Add( m_genres = new GenrSizer(this, database, sql), 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Language") );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	wxBoxSizer * bSizerFile = new wxBoxSizer( wxHORIZONTAL );

	m_lang.Create( this, wxID_ANY, result.GetString(wxT("lang")), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_lang, 1, wxALL, 3 );

	info = new wxStaticText( this, wxID_ANY, _("Extension") );
	info->Wrap( -1 );
	bSizerFile->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	m_type.Create( this, wxID_ANY, result.GetString(wxT("file_type")), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_type, 1, wxALL, 3 );

	info = new wxStaticText( this, wxID_ANY, _("Date") );
	info->Wrap( -1 );
	bSizerFile->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );

	FbDateTime date = result.GetInt(wxT("created"));
	m_date.Create( this, wxID_ANY, date, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerFile->Add( &m_date, 1, wxALL, 3 );

	fgSizerMain->Add( bSizerFile, 0, wxEXPAND, 0 );
*/
	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

//-----------------------------------------------------------------------------
//  FbGroupTitleDlg
//-----------------------------------------------------------------------------

bool FbGroupTitleDlg::Execute(const wxArrayInt &items)
{
	wxString codes = GetCodes(items);
	FbCommonDatabase database;
	FbGroupTitleDlg dlg(items, codes, database);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) dlg.Save(items, codes, database);
	return ok;
}

wxString FbGroupTitleDlg::GetCodes(const wxArrayInt &items)
{
	wxString result;
	size_t count = items.Count();
	for (size_t i = 0; i < count; i++) {
		if (i > 0) result << wxT(',');
		result << items[i];
	}
	return result;
}

FbGroupTitleDlg::FbGroupTitleDlg(const wxArrayInt &items, const wxString &codes, wxSQLite3Database &database)
	: FbTitleDlg(_("Group processing"))
{
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_title = new MainPanel( this, items, codes, database );
	sizer->Add( m_title, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizer->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	SetSizer( sizer );
	Layout();

	int h = GetParent()->GetSize().y;
	wxSize size = GetBestSize();
	size.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	size.y += 2;
	if (size.y > h) size.y = h;
	SetSize(size);

	Centre( wxBOTH );

	m_title->SetScrollbars(20, 20, 50, 50);
	m_title->Layout();
}

void FbGroupTitleDlg::Save(const wxArrayInt &items, const wxString &codes, wxSQLite3Database &database)
{
}
