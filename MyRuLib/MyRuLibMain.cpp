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
#include <wx/imaglist.h>
#include "MyRuLibMain.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "Books.h"
#include "Params.h"
#include "FbManager.h"
#include "FbGenres.h"

#include "res/new.xpm"
#include "res/find.xpm"
#include "res/new_dir.xpm"
#include "res/htmbook.xpm"
#include "res/checked.xpm"
#include "res/nocheck.xpm"

#define ID_LETTER_RU 30100
#define ID_LETTER_EN 30200

const wxString alphabetRu = _("АБВГДЕЖЗИКЛМНОПРСТУФХЦЧШЩЭЮЯ");
const wxString alphabetEn = _("#ABCDEFGHIJKLMNOPQRSTUVWXWZ");
wxString alphabet = alphabetRu + alphabetEn;

BEGIN_EVENT_TABLE(MyRuLibMainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyRuLibMainFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MyRuLibMainFrame::OnAbout)
    EVT_LISTBOX(ID_AUTHORS_LISTBOX, MyRuLibMainFrame::OnAuthorsListBoxSelected)
    EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListActivated)
	EVT_TREE_KEY_DOWN(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListKeyDown)
    EVT_HTML_LINK_CLICKED(ID_BOOKS_INFO_PANEL, MyRuLibMainFrame::OnBooksInfoPanelLinkClicked)
    EVT_TEXT_ENTER(ID_FIND_TEXT, MyRuLibMainFrame::OnFindTextEnter)
    EVT_TOOL(ID_FIND_BTN, MyRuLibMainFrame::OnFindTextEnter)
    EVT_TOOL(ID_NEW_FILE, MyRuLibMainFrame::OnNewFile)
    EVT_TOOL(ID_NEW_DIR, MyRuLibMainFrame::OnNewDir)
    EVT_TOOL(ID_NEW_ZIP, MyRuLibMainFrame::OnNewZip)
    EVT_MENU(ID_PROGRESS_START, MyRuLibMainFrame::OnProgressStart)
    EVT_MENU(ID_PROGRESS_UPDATE, MyRuLibMainFrame::OnProgressUpdate)
    EVT_MENU(ID_PROGRESS_FINISH, MyRuLibMainFrame::OnProgressFinish)
END_EVENT_TABLE()

class MyTreeListCtrl: public wxTreeListCtrl
{
public:
    MyTreeListCtrl(wxWindow *parent, wxWindowID id, long style)
        :wxTreeListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style) {};
    void OnSize(wxSizeEvent& event);
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyTreeListCtrl, wxTreeListCtrl)
    EVT_SIZE(MyTreeListCtrl::OnSize)
END_EVENT_TABLE()

void MyTreeListCtrl::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);
	w -= 20;
    SetColumnWidth(0, w * 5 / 8);
    SetColumnWidth(1, w * 2 / 8);
    SetColumnWidth(2, w / 8);
	event.Skip();
}

MyRuLibMainFrame::MyRuLibMainFrame() {
	Create(NULL, wxID_ANY, _("MyRuLib - My Russian Library"));
}

bool MyRuLibMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title)
{
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

	long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksListView = new MyTreeListCtrl(books_splitter, ID_BOOKS_LISTCTRL, style);
    m_BooksListView->AddColumn (_T("Заголовок"), 300, wxALIGN_LEFT);
    m_BooksListView->AddColumn (_T("Имя файла"), 100, wxALIGN_CENTER);
    m_BooksListView->AddColumn (_T("Размер, Кб"), 100, wxALIGN_RIGHT);
    m_BooksListView->SetColumnEditable (0, false);
    m_BooksListView->SetColumnEditable (1, false);
    m_BooksListView->SetColumnEditable (2, false);

    wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	m_BooksListView->AssignImageList (images);

	m_BooksInfoPanel = new wxHtmlWindow(books_splitter, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxSize(-1,-1), wxSUNKEN_BORDER);

	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BooksInfoPanel->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	books_splitter->SetSashGravity(1.0);

	splitter->SplitVertically(m_AuthorsListBox, books_splitter, 160);
	books_splitter->SplitHorizontally(m_BooksListView, m_BooksInfoPanel, books_splitter->GetSize().GetHeight()-220);

	FillAuthorsList(wxEmptyString);

	const int widths[] = {-92, -57, -35, -22};
    m_ProgressBar = new ProgressBar(this, ID_PROGRESSBAR);
    m_ProgressBar->SetFieldsCount(4);
	m_ProgressBar->SetStatusWidths(4, widths);
	SetStatusBar(m_ProgressBar);

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

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRowSet * allAuthors;

    const wxString orderBy = wxT("search_name");
	if (findText.IsEmpty()) {
        allAuthors = authors.All(orderBy);
    } else {
		wxString text = findText;
		FbManager::MakeLower(text);
        const wxString whereClause = wxString::Format(wxT("search_name like '%s%%'"), text.c_str());
        allAuthors = authors.WhereSet(whereClause, orderBy);
    }
	FillAuthorsList(allAuthors);

}
void MyRuLibMainFrame::FillAuthorsList(AuthorsRowSet * allAuthors) {

	m_AuthorsListBox->Freeze();
	m_AuthorsListBox->Clear();

	for(unsigned long i = 0; i < allAuthors->Count(); i++) {
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

class BookTreeItemData: public wxTreeItemData
{
public:
	BookTreeItemData(int id): m_id(id) { };
	int GetId() { return m_id; };
private:
	int m_id;
};

void MyRuLibMainFrame::FillBooksList(int author_id)
{
	m_BooksListView->Freeze();

    m_BooksListView->DeleteRoot();
    wxTreeItemId root = m_BooksListView->AddRoot (_T("Root"));

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * thisAuthor = authors.Id(author_id);
	if(thisAuthor)
	{
		BooksRowSet * allBooks = thisAuthor->GetBooks(wxT("title"));
		for(unsigned long i = 0; i < allBooks->Count(); ++i)
		{
		    BooksRow * thisBook = allBooks->Item(i);
			wxTreeItemId item = m_BooksListView->AppendItem(root, thisBook->title, -1, -1, new BookTreeItemData(thisBook->id));
			m_BooksListView->SetItemText (item, 1, thisBook->file_name);
			m_BooksListView->SetItemText (item, 2, wxString::Format(wxT("%d"), thisBook->file_size));
			m_BooksListView->SetItemImage(item, i%2);
			m_BooksListView->SetItemBold(item, i%3==1);
		}
		m_BooksInfoPanel->SetPage(wxT("<html><body></body></html>"));
	}
    m_BooksListView->ExpandAll(root);

	m_BooksListView->Thaw();
}

void MyRuLibMainFrame::OnAuthorsListBoxSelected(wxCommandEvent & event) {
	RecordIDClientData * data = (RecordIDClientData *)event.GetClientObject();
	if(data) {
		FillBooksList(data->GetID());
	}
}

void MyRuLibMainFrame::OnBooksListViewSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BooksListView->GetItemData(selected);
        Books books(wxGetApp().GetDatabase());
        wxString whereClause = wxString::Format(wxT("id=%d"), data->GetId());
        BooksRowSet * allBooks = books.WhereSet( whereClause, wxT("title"));
        wxString title, annotation, genres;
		wxStringList authorList;
		for(size_t i = 0; i < allBooks->Count(); ++i) {
		    BooksRow * thisBook = allBooks->Item(i);
			Authors authors(wxGetApp().GetDatabase());
		    title = thisBook->title;
            genres = thisBook->genres;
			if (!thisBook->annotation.IsEmpty()) {
				annotation = thisBook->annotation;
			}
			authorList.Add(authors.Id(thisBook->id_author)->full_name);
		}
		authorList.Sort();
		wxString authorText, genreText;
		for (size_t i = 0; i<authorList.GetCount(); i++) {
			if (!authorText.IsEmpty())
				authorText += wxT(", ");
			authorText += authorList[i];
		}
		for (size_t i = 0; i<genres.Len(); i++) {
			if (!genreText.IsEmpty())
				genreText += wxT(", ");
			genreText +=  FbGenres::Name(genres[i]);
		}

		wxString html(wxT("<html><body>"));

        html += wxString::Format(wxT("<font size=4><b>%s</b></font>"), authorText.c_str());

		if (!genreText.IsEmpty())
            html += wxString::Format(wxT("<br><font size=3>%s</font>"), genreText.c_str());

        html += wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), title.c_str());

        html += annotation;

        html += wxT("</body></html>");

		m_BooksInfoPanel->SetPage(html);
	}
	event.Skip();
}

void MyRuLibMainFrame::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		int image = (m_BooksListView->GetItemImage(selected) + 1) % 2;
		m_BooksListView->SetItemImage(selected, image);
	}
	event.Skip();
}

void MyRuLibMainFrame::OnBooksListKeyDown(wxTreeEvent & event)
{
	if (event.GetKeyCode() == 0x20) {
		wxArrayTreeItemIds selections;
		size_t count = m_BooksListView->GetSelections(selections);
		int image = 0;
		for (size_t i=0; i<count; ++i) {
            wxTreeItemId selected = selections[i];
		    if (i==0)
                image = (m_BooksListView->GetItemImage(selected) + 1) % 2;
			m_BooksListView->SetItemImage(selected, image);
		}
		event.Veto();
	} else
		event.Skip();
}

void MyRuLibMainFrame::OnBooksInfoPanelLinkClicked(wxHtmlLinkEvent & event)
{
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

	Authors authors(wxGetApp().GetDatabase());
    AuthorsRowSet * allAuthors = authors.WhereSet(whereClause, orderBy);
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

void MyRuLibMainFrame::OnProgressStart(wxCommandEvent& event)
{
	wxFileName filename(event.GetString());
	m_StatusText = filename.GetFullName();
	m_ProgressBar->SetRange(event.GetInt());
	m_ProgressBar->SetStatusText(m_StatusText, 0);
	m_ProgressBar->SetStatusText(wxEmptyString, 2);
}

void MyRuLibMainFrame::OnProgressUpdate(wxCommandEvent& event)
{
	m_ProgressBar->SetProgress(event.GetInt());
	m_ProgressBar->SetStatusText(m_StatusText, 0);
	m_ProgressBar->SetStatusText(event.GetString(), 2);
}

void MyRuLibMainFrame::OnProgressFinish(wxCommandEvent& event)
{
	wxFileName filename(event.GetString());
	m_StatusText = filename.GetFullName();
	m_ProgressBar->SetProgress(0);
	m_ProgressBar->SetStatusText(wxEmptyString, 0);
	m_ProgressBar->SetStatusText(wxEmptyString, 2);
}

