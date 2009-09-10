#include "FbFrameAuthor.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbParams.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "RecordIDClientData.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"

BEGIN_EVENT_TABLE(FbFrameAuthor, FbFrameBase)
    EVT_LISTBOX(ID_AUTHORS_LISTBOX, FbFrameAuthor::OnAuthorsListBoxSelected)
END_EVENT_TABLE()

FbFrameAuthor::FbFrameAuthor(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameAuthor::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(m_RuAlphabar = CreateAlphaBar(this, alphabetRu, ID_LETTER_RU, wxTB_NODIVIDER), 0, wxEXPAND, 5);
	sizer->Add(m_EnAlphabar = CreateAlphaBar(this, alphabetEn, ID_LETTER_EN, 0), 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_AuthorsListBox = new FbAuthorList(splitter, ID_AUTHORS_LISTBOX);
	m_AuthorsListBox->SetFocus();

	long substyle = wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksPanel.Create(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER, substyle);
	m_BooksPanel.CreateAuthorColumns();
	splitter->SplitVertically(m_AuthorsListBox, &m_BooksPanel, 160);

	SetMenuBar(CreateMenuBar());
	Layout();
}

wxToolBar * FbFrameAuthor::CreateAlphaBar(wxWindow * parent, const wxString & alphabet, const int &toolid, long style)
{
	wxToolBar * toolBar = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_NOICONS|style);
	for (size_t i = 0; i<alphabet.Len(); i++) {
	    wxString letter = alphabet.Mid(i, 1);
	    int btnid = toolid + i;
        toolBar->AddTool(btnid, letter, wxNullBitmap, wxNullBitmap, wxITEM_CHECK)->SetClientData( (wxObject*) i);
        this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( FbFrameAuthor::OnLetterClicked ) );
	}
	toolBar->Realize();
	return toolBar;
}

void FbFrameAuthor::ToggleAlphabar(const int &idLetter)
{
	for (size_t i = 0; i<alphabetRu.Len(); i++) {
	    int id = ID_LETTER_RU + i;
        m_RuAlphabar->ToggleTool(id, id == idLetter);
	}
	for (size_t i = 0; i<alphabetEn.Len(); i++) {
	    int id = ID_LETTER_EN + i;
        m_EnAlphabar->ToggleTool(id, id == idLetter);
	}
}

void FbFrameAuthor::OnLetterClicked( wxCommandEvent& event )
{
    int id = event.GetId();

    wxString alphabet;
	size_t position;

    if (id < ID_LETTER_EN) {
        alphabet = alphabetRu;
        position = id - ID_LETTER_RU;
    } else {
        alphabet = alphabetEn;
        position = id - ID_LETTER_EN;
    };

    ToggleAlphabar(id);

	m_AuthorsListBox->FillAuthorsChar(alphabet[position]);
	SelectFirstAuthor();
}

void FbFrameAuthor::SelectFirstAuthor()
{
	if(m_AuthorsListBox->GetCount()) {
		m_AuthorsListBox->SetSelection(0);
		RecordIDClientData * data = (RecordIDClientData *) m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
        if (data) m_BooksPanel.FillByAuthor(data->GetID());
	} else {
		m_BooksPanel.m_BookList->DeleteRoot();
		m_BooksPanel.m_BookInfo->SetPage(wxEmptyString);
	}
}

void FbFrameAuthor::OnAuthorsListBoxSelected(wxCommandEvent & event)
{
	RecordIDClientData * data = (RecordIDClientData *)event.GetClientObject();
	if (data) m_BooksPanel.FillByAuthor(data->GetID());
}

void FbFrameAuthor::ActivateAuthors()
{
    m_AuthorsListBox->SetFocus();
}

void FbFrameAuthor::FindAuthor(const wxString &text)
{
	if (text.IsEmpty()) return;
    ToggleAlphabar(0);
    m_AuthorsListBox->FillAuthorsText(text);
    SelectFirstAuthor();
}

void FbFrameAuthor::SelectRandomLetter()
{
    wxDateTime now = wxDateTime::Now();
    int random = now.GetHour() * 60 * 60 + now.GetMinute() * 60 + now.GetSecond();
	random = random % alphabetRu.Len();

    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_LETTER_RU + random );
    wxPostEvent(this, event);
}
