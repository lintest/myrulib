#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbParams.h"
#include "FbManager.h"
#include "BooksPanel.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
	EVT_MENU(wxID_SELECTALL, FbFrameSearch::OnSubmenu)
    EVT_MENU(wxID_SAVE, FbFrameSearch::OnSubmenu)
    EVT_MENU(ID_SPLIT_HORIZONTAL, FbFrameSearch::OnSubmenu)
    EVT_MENU(ID_SPLIT_VERTICAL, FbFrameSearch::OnSubmenu)
    EVT_MENU(ID_BOOKINFO_UPDATE, FbFrameSearch::OnSubmenu)
    EVT_UPDATE_UI(ID_SPLIT_HORIZONTAL, FbFrameSearch::OnChangeViewUpdateUI)
    EVT_UPDATE_UI(ID_SPLIT_VERTICAL, FbFrameSearch::OnChangeViewUpdateUI)
END_EVENT_TABLE()

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameSearch::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxStaticText* m_staticText1;
    wxStaticText* m_staticText2;
    wxToolBar* m_toolBar1;

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Заголовок:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textTitle = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textTitle, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Автор:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textAuthor = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textAuthor, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORZ_TEXT | wxTB_NODIVIDER);
	m_toolBar1->AddTool( ID_SEARCH_BOOK, _(" Найти "), wxArtProvider::GetBitmap(wxART_FIND), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->Realize();

	bSizer2->Add( m_toolBar1, 0, wxEXPAND|wxALL, 5 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	m_BooksPanel = new BooksPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER);
	m_BooksPanel->CreateSearchColumns();
	bSizer1->Add( m_BooksPanel, 1, wxEXPAND, 5 );

	SetMenuBar(CreateMenuBar());

	SetSizer( bSizer1 );
	Layout();
}

void FbFrameSearch::OnSubmenu(wxCommandEvent& event)
{
    wxPostEvent(m_BooksPanel, event);
}

void FbFrameSearch::OnChangeViewUpdateUI(wxUpdateUIEvent & event)
{
    if (event.GetId() == ID_SPLIT_HORIZONTAL)
        event.Check(m_BooksPanel->GetSplitMode() == wxSPLIT_HORIZONTAL);
    else
        event.Check(m_BooksPanel->GetSplitMode() == wxSPLIT_VERTICAL);
}
