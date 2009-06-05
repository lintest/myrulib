/***************************************************************
 * Name:      MyRuLibMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/splitter.h>
#include <wx/aboutdlg.h>
#include "MyRuLibMain.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "Books.h"
#include "Params.h"
#include "FbManager.h"

#include "res/new.xpm"
#include "res/find.xpm"
#include "res/new_dir.xpm"
#include "res/htmbook.xpm"  

enum {
	ID_AUTHORS_LISTBOX = 10001,
	ID_BOOKS_LISTCTRL,
	ID_BOOKS_INFO_PANEL,
	ID_NEW_FILE,
	ID_NEW_DIR,
	ID_NEW_ZIP,
	ID_FIND_TEXT,
	ID_FIND_BTN,
};

#define ID_LETTER_RU 30100
#define ID_LETTER_EN 30200

const wxString alphabetRu = _("#АБВГДЕЖЗИКЛМНОПРСТУФХЦЧШЩЭЮЯ");
const wxString alphabetEn = _("#ABCDEFGHIJKLMNOPQRSTUVWXWZ");
wxString alphabet = alphabetRu + alphabetEn;

BEGIN_EVENT_TABLE(MyRuLibMainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyRuLibMainFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MyRuLibMainFrame::OnAbout)
    EVT_LISTBOX(ID_AUTHORS_LISTBOX, MyRuLibMainFrame::OnAuthorsListBoxSelected)
    EVT_LIST_ITEM_SELECTED(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListViewSelected)
    EVT_SIZE(MyRuLibMainFrame::OnBooksListViewResize)
    EVT_HTML_LINK_CLICKED(ID_BOOKS_INFO_PANEL, MyRuLibMainFrame::OnBooksInfoPanelLinkClicked)
    EVT_TEXT_ENTER(ID_FIND_TEXT, MyRuLibMainFrame::OnFindTextEnter)
    EVT_TOOL(ID_FIND_BTN, MyRuLibMainFrame::OnFindTextEnter)
    EVT_TOOL(ID_NEW_FILE, MyRuLibMainFrame::OnNewFile)
    EVT_TOOL(ID_NEW_DIR, MyRuLibMainFrame::OnNewDir)
    EVT_TOOL(ID_NEW_ZIP, MyRuLibMainFrame::OnNewZip)
END_EVENT_TABLE()

MyRuLibMainFrame::MyRuLibMainFrame() {
	Create(NULL, wxID_ANY, _("MyRuLib - My Russian Library"));
}

bool MyRuLibMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title) {
//	alphabet = alphabetRu + alphabetEn;
	bool res = wxFrame::Create(parent, id, title, wxDefaultPosition, wxSize(700, 500));
	if(res)	{
		CreateControls();
		wxIcon icon(wxT("aaaa"));
		SetIcon(icon);
	}
	return res;
}

void MyRuLibMainFrame::CreateControls() {
	wxMenuBar * menuBar = new wxMenuBar;
	SetMenuBar(menuBar);

	wxMenu * fileMenu = new wxMenu;
	fileMenu->Append(ID_NEW_FILE, _("Добавить файл…"));
	fileMenu->Append(ID_NEW_DIR, _("Добавить директорию…"));
	fileMenu->Append(ID_NEW_ZIP, _("Добавить файл ZIP…"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, _("Выход\tAlt+F4"));
	menuBar->Append(fileMenu, _("&Файл"));

	wxMenu * helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, _("О программе…"));
	menuBar->Append(helpMenu, _("&?"));

	SetToolBar(CreateButtonBar());

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(CreateAlphaBar(alphabetRu, ID_LETTER_RU), 0, wxEXPAND, 5);
	sizer->Add(CreateAlphaBar(alphabetEn, ID_LETTER_EN), 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(100);
	sizer->Add(splitter, 1, wxEXPAND);

	m_AuthorsListBox = new wxListBox(splitter, ID_AUTHORS_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, NULL, wxSUNKEN_BORDER);

	wxSplitterWindow * books_splitter = new wxSplitterWindow(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	books_splitter->SetMinimumPaneSize(100);
	m_BooksListView = new wxListView(books_splitter, ID_BOOKS_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER);
	m_BooksListView->InsertColumn(0, _("Заголовок"), wxLIST_FORMAT_LEFT, 300);
	m_BooksListView->InsertColumn(1, _("Имя файла"), wxLIST_FORMAT_LEFT, 100);
	m_BooksListView->InsertColumn(2, _("Размер, Кб"), wxLIST_FORMAT_RIGHT, 100);

	m_BooksInfoPanel = new wxHtmlWindow(books_splitter, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxSize(-1,-1), wxSUNKEN_BORDER);

	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BooksInfoPanel->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	books_splitter->SetSashGravity(1.0);

	splitter->SplitVertically(m_AuthorsListBox, books_splitter, 160);
	books_splitter->SplitHorizontally(m_BooksListView, m_BooksInfoPanel, books_splitter->GetSize().GetHeight()-220);

	FillAuthorsList(wxEmptyString);

	CreateStatusBar(2);
	Centre();
}

void MyRuLibMainFrame::OnAbout(wxCommandEvent & event)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("MyRuLib"));
	info.SetVersion(wxT("0.1"));
	info.SetWebSite(wxT("http://lintest.ru"));
	info.AddDeveloper(wxT("Kandrashin Denis"));
	wxAboutBox(info);
}

wxToolBar * MyRuLibMainFrame::CreateButtonBar() {
	wxToolBar * toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT);
	toolBar->AddTool(ID_NEW_FILE, _("Файл…"), wxBitmap(new_xpm));
	toolBar->AddTool(ID_NEW_DIR, _("Папка…"), wxBitmap(new_dir_xpm));
	toolBar->AddTool(ID_NEW_ZIP, _("Zip файл…"), wxBitmap(htmbook_xpm));
	toolBar->AddSeparator();
	m_FindTextCtrl = new wxTextCtrl( toolBar, ID_FIND_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	toolBar->AddControl( m_FindTextCtrl );
	toolBar->AddTool(ID_FIND_BTN, _("Найти"), wxBitmap(find_xpm));
	toolBar->Realize();
	return toolBar;
}

wxToolBar * MyRuLibMainFrame::CreateAlphaBar(const wxString & alphabet, int toolid) {
	wxToolBar * toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_NOICONS);
	size_t iLength = alphabet.Len();
	for (size_t i = 0; i<iLength; i++) {
	    wxString letter = alphabet.Mid(i, 1);
	    int btnid = toolid + i;
        toolBar->AddTool(btnid, letter, wxNullBitmap)->SetClientData( (wxObject*) i);
        this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MyRuLibMainFrame::OnLetterClicked ) );
	}
	toolBar->Realize();
	return toolBar;
}

void MyRuLibMainFrame::OnExit(wxCommandEvent & event) {
	wxUnusedVar(event);
	Close();
}

void MyRuLibMainFrame::FillAuthorsList(const wxString &findText) {

    AuthorsRowSet * allAuthors;
    const wxString orderBy = wxT("search_name");
    if (findText == wxEmptyString) {
        allAuthors = wxGetApp().GetAuthors()->All(orderBy);
    } else {
		wxString text = findText;
		FbManager::MakeLower(text);
        const wxString whereClause = wxString::Format(wxT("search_name like '%s%%'"), text.c_str());
        allAuthors = wxGetApp().GetAuthors()->WhereSet(whereClause, orderBy);
    }
	FillAuthorsList(allAuthors);

}
void MyRuLibMainFrame::FillAuthorsList(AuthorsRowSet * allAuthors) {

	m_AuthorsListBox->Freeze();
	m_AuthorsListBox->Clear();

	for(unsigned long i = 0; i < allAuthors->Count(); ++i) {
		m_AuthorsListBox->Append(allAuthors->Item(i)->full_name,
			new RecordIDClientData(allAuthors->Item(i)->id));
	}

	if(m_AuthorsListBox->GetCount()) {
		m_AuthorsListBox->SetSelection(0);
		RecordIDClientData * data = (RecordIDClientData *)
			m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
		if(data) {
			FillBooksList(data->GetID());
		}
	}
	m_AuthorsListBox->Thaw();
}

void MyRuLibMainFrame::FillBooksList(int author_id) 
{
	m_BooksListView->Freeze();
	m_BooksListView->DeleteAllItems();
	AuthorsRow * thisAuthor = wxGetApp().GetAuthors()->Id(author_id);
	if(thisAuthor)
	{
		BooksRowSet * allBooks = thisAuthor->GetBooks(wxT("title"));
		long item(0);
		for(unsigned long i = 0; i < allBooks->Count(); ++i)
		{
		    BooksRow * thisBook = allBooks->Item(i);
			item = m_BooksListView->InsertItem(item + 1, thisBook->title);
			m_BooksListView->SetItem(item, 1, thisBook->file_name);
			m_BooksListView->SetItem(item, 2, wxString::Format(wxT("%d"), thisBook->file_size));
			m_BooksListView->SetItemData(item, (long)thisBook->id);
		}
		if(m_BooksListView->GetItemCount())
		{
			m_BooksListView->Select(0);
		}
		else
		{
			m_BooksInfoPanel->SetPage(wxT("<html><body></body></html>"));
		}
	}
	m_BooksListView->Thaw();
}

void MyRuLibMainFrame::OnAuthorsListBoxSelected(wxCommandEvent & event) {
	RecordIDClientData * data = (RecordIDClientData *)event.GetClientObject();
	if(data) {
		FillBooksList(data->GetID());
	}
}

void MyRuLibMainFrame::OnBooksListViewSelected(wxListEvent & event) {
}

void MyRuLibMainFrame::OnBooksInfoPanelLinkClicked(wxHtmlLinkEvent & event) {
}

void MyRuLibMainFrame::OnFindTextEnter( wxCommandEvent& event ){
    FillAuthorsList(m_FindTextCtrl->GetValue());
}

void MyRuLibMainFrame::OnLetterClicked( wxCommandEvent& event ){

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

    wxString letter = alphabet.Mid(position, 1);

    const wxString orderBy = wxT("search_name");
    const wxString whereClause = wxString::Format(wxT("letter = '%s'"), letter.c_str());
    AuthorsRowSet * allAuthors = wxGetApp().GetAuthors()->WhereSet(whereClause, orderBy);
	FillAuthorsList(allAuthors);
}

void MyRuLibMainFrame::OnNewFile( wxCommandEvent& event ){

    wxFileDialog dlg (
		this,
		_("Выберите файл для добавления в библиотеку…"),
		wxEmptyString,
		wxEmptyString,
		_("Fiction books (*.fb2)|*.fb2"),
		wxFD_OPEN | wxFD_MULTIPLE,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		m_BooksInfoPanel->SetPage(wxT("<html><body></body></html>"));

		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			wxString filename = paths[i];
			SetTitle(filename);
			wxString html;
			FbManager parser;
			parser.ParseXml(filename, html);
			m_BooksInfoPanel->AppendToPage(html);
		}
	}
}

void MyRuLibMainFrame::OnNewDir( wxCommandEvent& event ){

    wxDirDialog dlg(
        this,
        _("Выберите директорию для импорта файлов"),
        wxEmptyString,
        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST
    );

	if (dlg.ShowModal() == wxID_OK) {
		SetTitle(dlg.GetPath());
	}
}

void MyRuLibMainFrame::OnNewZip( wxCommandEvent& event ){

    wxFileDialog dlg (
		this,
		_("Выберите zip-файл для добавления в библиотеку…"),
		wxEmptyString,
		wxEmptyString,
		_("Zip file (*.zip)|*.zip"),
		wxFD_OPEN | wxFD_MULTIPLE,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		m_BooksInfoPanel->SetPage(wxT("<html><body></body></html>"));

		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			wxString filename = paths[i];
			SetTitle(filename);
			wxString html;
			FbManager parser;
			parser.ParseZip(filename, html);
			m_BooksInfoPanel->AppendToPage(html);
		}
	}
}

void MyRuLibMainFrame::OnBooksListViewResize(wxSizeEvent& event)
{
	event.Skip();
}

