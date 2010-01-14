#include "FbFilterDlg.h"
#include "FbBookList.h"

///////////////////////////////////////////////////////////////////////////

FbFilterDlg::FbFilterDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerOwner;
	sbSizerOwner = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Отбор книг по принадлежности") ), wxHORIZONTAL );

	m_checkLib = new wxCheckBox( this, ID_CHECK_LIB, _("Библиотечные"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOwner->Add( m_checkLib, 0, wxALL|wxEXPAND, 5 );

	m_checkUsr = new wxCheckBox( this, ID_CHECK_USR, _("Собственные"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOwner->Add( m_checkUsr, 0, wxALL|wxEXPAND, 5 );

	bSizerMain->Add( sbSizerOwner, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizerList;
	bSizerList = new wxBoxSizer( wxHORIZONTAL );

	m_treeLang = new FbBookList( this, ID_TREE_LANG, wxTR_FULL_ROW_HIGHLIGHT | wxTR_MULTIPLE | wxSUNKEN_BORDER );
	m_treeLang->SetMinSize( wxSize( 100,100 ) );
	bSizerList->Add( m_treeLang, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	m_treeLang->AddColumn (_("Язык"), 10, wxALIGN_LEFT);
	m_treeLang->AddRoot(wxT("Все"));

	m_treeType = new FbBookList( this, ID_TREE_TYPE, wxTR_FULL_ROW_HIGHLIGHT | wxTR_MULTIPLE | wxSUNKEN_BORDER );
	m_treeType->SetMinSize( wxSize( 100,100 ) );
	bSizerList->Add( m_treeType, 1, wxEXPAND|wxALL, 5 );
	m_treeType->AddColumn (_("Тип файла"), 10, wxALIGN_LEFT);
	m_treeType->AddRoot(wxT("Все"));

	bSizerMain->Add( bSizerList, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizer = new wxStdDialogButtonSizer();
	wxButton * sdbSizerYes = new wxButton( this, wxID_YES );
	sdbSizer->AddButton( sdbSizerYes );
	sdbSizerYes->SetDefault();
	wxButton * sdbSizerNo = new wxButton( this, wxID_NO );
	sdbSizer->AddButton( sdbSizerNo );
	wxButton * sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	sdbSizer->AddButton( sdbSizerCancel );
	sdbSizer->Realize();
	bSizerMain->Add( sdbSizer, 0, wxEXPAND|wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	SetAffirmativeId(wxID_YES);
	SetEscapeId(wxID_CANCEL);
}

FbFilterDlg::~FbFilterDlg()
{
}
