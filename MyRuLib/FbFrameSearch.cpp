#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbParams.h"
#include "FbManager.h"
#include "BooksPanel.h"

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

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Наименование:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textTitle = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textTitle, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Автор:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textAuthor = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textAuthor, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT );
	m_toolBar1->AddTool( ID_SEARCH_BOOK, _(" Найти "), wxArtProvider::GetBitmap(wxART_FIND), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->Realize();

	bSizer2->Add( m_toolBar1, 0, wxEXPAND|wxALL, 5 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	wxListBox * m_AuthorsListBox = new wxListBox(splitter, ID_AUTHORS_LISTBOX);

	wxSplitterWindow * m_bookspanel = new BooksPanel(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SplitVertically(m_AuthorsListBox, m_bookspanel, 160);

//	CreateBookInfo();

	SetMenuBar(CreateMenuBar());

	this->SetSizer( bSizer1 );
	this->Layout();
}

void FbFrameSearch::CreateBookInfo()
{
    int vertical = FbParams().GetValue(FB_VIEW_TYPE);

	if (m_BooksInfoPanel) m_BooksSplitter->Unsplit(m_BooksInfoPanel);

	m_BooksInfoPanel = new wxHtmlWindow(m_BooksSplitter, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BooksInfoPanel->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	if (vertical)
		m_BooksSplitter->SplitVertically(m_BooksListView, m_BooksInfoPanel, m_BooksSplitter->GetSize().GetWidth()/2);
	else
		m_BooksSplitter->SplitHorizontally(m_BooksListView, m_BooksInfoPanel, m_BooksSplitter->GetSize().GetHeight()/2);
/*
    BookTreeItemData * book = GetSelectedBook();
    if (!book) {
        m_BooksInfoPanel->SetPage(wxEmptyString);
    } else {
        wxString html = InfoCash::GetInfo(book->GetId(), vertical);
        m_BooksInfoPanel->SetPage(html);
        InfoCash::ShowInfo(this, book->GetId(), book->file_type);
    }
*/

}

void FbFrameSearch::OnActivated(wxActivateEvent & event)
{
    /*
	AUIDocViewMainFrame * frame = wxDynamicCast(GetMDIParentFrame(),
		AUIDocViewMainFrame);
	if(frame)
	{
		frame->GetLOGTextCtrl()->SetValue(wxString::Format(
			_("Some help text about '%s'"),	GetTitle().GetData()));
	}
	*/
}

