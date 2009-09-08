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

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	SetMenuBar(CreateMenuBar());

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( toolbar, 0, wxGROW);

	m_BooksPanel = new BooksPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER);
	m_BooksPanel->CreateSearchColumns();
	bSizer1->Add( m_BooksPanel, 1, wxEXPAND, 5 );

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
/*
wxToolBar * FbFrameSearch::GetToolBar() const
{
    wxToolBar * toolbar = new wxToolBar(this, wxID_ANY);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));

    return toolbar;
};
*/

wxToolBar * FbFrameSearch::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
    wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();
    return toolbar;
}
