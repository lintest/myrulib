/***************************************************************
 * Name:      MyRuLibMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/splitter.h>
#include <wx/imaglist.h>
#include "MyRuLibMain.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "Books.h"
#include "Params.h"
#include "FbManager.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "BookList.h"
#include "SettingsDlg.h"
#include "FbThread.h"

#include "XpmBitmaps.h"

#define ID_LETTER_RU 30100
#define ID_LETTER_EN 30200

wxString alphabetRu = _("АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
wxString alphabetEn = wxT("#ABCDEFGHIJKLMNOPQRSTUVWXWZ");
wxString blank_page = wxT("<html><body></body></html>");
wxString strAlphabet = alphabetRu + alphabetEn;
wxString strNobody = _("(без автора)");
wxString strRusJE = wxT("е");
wxString strRusJO = wxT("ё");
wxString strParsingInfo = wxT("Обработка файла: ");
wxString strOtherSequence = wxT("(прочие)");
wxString strBookNotFound = wxT("Не найден архив%s, содержащий файл%s.");

BEGIN_EVENT_TABLE(MyRuLibMainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyRuLibMainFrame::OnExit)
	EVT_MENU(wxID_SETUP, MyRuLibMainFrame::OnSetup)
	EVT_MENU(ID_SPLIT_HORIZONTAL, MyRuLibMainFrame::OnChangeView)
	EVT_MENU(ID_SPLIT_VERTICAL, MyRuLibMainFrame::OnChangeView)
	EVT_MENU(wxID_ABOUT, MyRuLibMainFrame::OnAbout)
    EVT_LISTBOX(ID_AUTHORS_LISTBOX, MyRuLibMainFrame::OnAuthorsListBoxSelected)
    EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListActivated)
	EVT_TREE_KEY_DOWN(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnBooksListKeyDown)
	EVT_TREE_STATE_IMAGE_CLICK(ID_BOOKS_LISTCTRL, MyRuLibMainFrame::OnImageClick)
    EVT_HTML_LINK_CLICKED(ID_BOOKS_INFO_PANEL, MyRuLibMainFrame::OnBooksInfoPanelLinkClicked)
    EVT_TEXT_ENTER(ID_FIND_TEXT, MyRuLibMainFrame::OnFindTextEnter)
    EVT_TOOL(ID_FIND_BTN, MyRuLibMainFrame::OnFindTextEnter)
    EVT_TOOL(ID_NEW_FILE, MyRuLibMainFrame::OnNewFile)
    EVT_TOOL(ID_NEW_DIR, MyRuLibMainFrame::OnNewDir)
    EVT_TOOL(ID_NEW_ZIP, MyRuLibMainFrame::OnNewZip)
    EVT_TOOL(ID_REG_ZIP, MyRuLibMainFrame::OnRegZip)
    EVT_MENU(ID_PROGRESS_START, MyRuLibMainFrame::OnProgressStart)
    EVT_MENU(ID_PROGRESS_UPDATE, MyRuLibMainFrame::OnProgressUpdate)
    EVT_MENU(ID_PROGRESS_FINISH, MyRuLibMainFrame::OnProgressFinish)
    EVT_MENU(ID_SET_ANNOTATION, MyRuLibMainFrame::OnSetAnnotation)
END_EVENT_TABLE()

MyRuLibMainFrame::MyRuLibMainFrame()
	:m_BooksInfoPanel(NULL), m_id_book(0)
{
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

void MyRuLibMainFrame::CreateControls()
{
	wxMenuBar * menuBar = new wxMenuBar;

	wxMenu * fileMenu = new wxMenu;
	fileMenu->Append(ID_NEW_FILE, _("Добавить файл…"))->SetBitmap(wxBitmap(new_xpm));
	fileMenu->Append(ID_NEW_DIR, _("Добавить директорию…"))->SetBitmap(wxBitmap(new_dir_xpm));
	fileMenu->Append(ID_NEW_ZIP, _("Добавить файл ZIP…"))->SetBitmap(wxBitmap(htmbook_xpm));
	fileMenu->Append(ID_REG_ZIP, _("Зарегистрировать ZIP…"))->SetBitmap(wxBitmap(htmbook_xpm));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, _("Выход\tAlt+F4"));
	menuBar->Append(fileMenu, _("&Файл"));

	wxMenu * serviceMenu = new wxMenu;
	serviceMenu->Append(wxID_SETUP, _("Настройки"));
	menuBar->Append(serviceMenu, _("&Сервис"));

	wxMenu * viewMenu = new wxMenu;
	viewMenu->Append(ID_SPLIT_HORIZONTAL, _("&Просмотр справа"));
	viewMenu->Append(ID_SPLIT_VERTICAL, _("&Просмтр снизу"));
	menuBar->Append(viewMenu, _("&Вид"));

	wxMenu * helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, _("О программе…"));
	menuBar->Append(helpMenu, _("&?"));

	SetMenuBar(menuBar);

	SetToolBar(CreateButtonBar());

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(CreateAlphaBar(alphabetRu, ID_LETTER_RU), 0, wxEXPAND, 5);
	sizer->Add(CreateAlphaBar(alphabetEn, ID_LETTER_EN), 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(100);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_AuthorsListBox = new wxListBox(splitter, ID_AUTHORS_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, NULL, wxSUNKEN_BORDER);

	m_books_splitter = new wxSplitterWindow(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	m_books_splitter->SetMinimumPaneSize(100);
	m_books_splitter->SetSashGravity(0.5);

	long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksListView = new BookListCtrl(m_books_splitter, ID_BOOKS_LISTCTRL, style);
    m_BooksListView->AddColumn (_T("Заголовок"), 300, wxALIGN_LEFT);
    m_BooksListView->AddColumn (_T("№"), 30, wxALIGN_LEFT);
    m_BooksListView->AddColumn (_T("Имя файла"), 100, wxALIGN_LEFT);
    m_BooksListView->AddColumn (_T("Размер, Кб"), 100, wxALIGN_RIGHT);
    m_BooksListView->SetColumnEditable (0, false);
    m_BooksListView->SetColumnEditable (1, false);
    m_BooksListView->SetColumnEditable (2, false);
    m_BooksListView->SetColumnEditable (3, false);

    wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	images->Add (wxBitmap(checkout_xpm));
	m_BooksListView->AssignImageList (images);

	splitter->SplitVertically(m_AuthorsListBox, m_books_splitter, 160);

	CreateBookInfo(FbParams().GetValue(FB_VIEW_TYPE));

	FbManager::FillAuthorsChar(m_AuthorsListBox, _("А")[0]);

	const int widths[] = {-92, -57, -35, -22};
    m_ProgressBar = new ProgressBar(this, ID_PROGRESSBAR);
    m_ProgressBar->SetFieldsCount(4);
	m_ProgressBar->SetStatusWidths(4, widths);
	SetStatusBar(m_ProgressBar);

	Centre();
}

void MyRuLibMainFrame::CreateBookInfo(bool vertical)
{
	if (m_BooksInfoPanel) m_books_splitter->Unsplit(m_BooksInfoPanel);

	m_BooksInfoPanel = new wxHtmlWindow(m_books_splitter, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxSize(-1,-1), wxSUNKEN_BORDER);
	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BooksInfoPanel->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	if (vertical)
		m_books_splitter->SplitVertically(m_BooksListView, m_BooksInfoPanel, m_books_splitter->GetSize().GetWidth()/2);
	else
		m_books_splitter->SplitHorizontally(m_BooksListView, m_BooksInfoPanel, m_books_splitter->GetSize().GetHeight()/2);

	m_BooksInfoPanel->SetPage(m_html);
	if (m_id_book) InfoThread::Execute(m_id_book);

}

void MyRuLibMainFrame::OnSetup(wxCommandEvent & event)
{
    SettingsDlg::Execute(this);
}

void MyRuLibMainFrame::OnChangeView(wxCommandEvent & event)
{
	switch (event.GetId()) {
		case ID_SPLIT_HORIZONTAL:
			CreateBookInfo(true);
			break;
		case ID_SPLIT_VERTICAL:
			CreateBookInfo(false);
			break;
	}

	FbParams params;
	params.SetValue(FB_VIEW_TYPE, event.GetId() == ID_SPLIT_HORIZONTAL);
}

void MyRuLibMainFrame::OnAbout(wxCommandEvent & event)
{

    wxMessageBox(_T("MyRuLib About..."));
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

void MyRuLibMainFrame::OnAuthorsListBoxSelected(wxCommandEvent & event)
{
	RecordIDClientData * data = (RecordIDClientData *)event.GetClientObject();
	if(data) {
		FbManager::FillBooks(m_BooksListView, data->GetID());
		m_BooksInfoPanel->SetPage(blank_page);
		m_html.Empty();
	}
}

void MyRuLibMainFrame::OnBooksListViewSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data) {
		    m_id_book = data->GetId();
            m_html = FbManager::GetBookInfo(m_id_book);
            InfoThread::Execute(m_id_book);
		} else {
		    m_id_book = 0;
		    m_html = (wxString)wxEmptyString;
		}
        m_BooksInfoPanel->SetPage(m_html);
	}
	event.Skip();
}

void MyRuLibMainFrame::OnBooksInfoPanelLinkClicked(wxHtmlLinkEvent & event)
{
}

void MyRuLibMainFrame::SelectFirstAuthor()
{
	if(m_AuthorsListBox->GetCount()) {
		m_AuthorsListBox->SetSelection(0);
		RecordIDClientData * data = (RecordIDClientData *)
			m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
		if(data) {
			FbManager::FillBooks(m_BooksListView, data->GetID());
			m_BooksInfoPanel->SetPage(blank_page);
			m_html.Empty();
		}
	} else {
		m_BooksListView->DeleteRoot();
		m_BooksInfoPanel->SetPage(blank_page);
		m_html.Empty();
	}
}

void MyRuLibMainFrame::OnFindTextEnter( wxCommandEvent& event )
{
	if (!m_FindTextCtrl->GetValue().IsEmpty()) {
		FbManager::FillAuthorsText(m_AuthorsListBox, m_FindTextCtrl->GetValue());
		SelectFirstAuthor();
	}
}

void MyRuLibMainFrame::OnLetterClicked( wxCommandEvent& event )
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

	FbManager::FillAuthorsChar(m_AuthorsListBox, alphabet[position]);
	SelectFirstAuthor();
}

void MyRuLibMainFrame::OnNewFile( wxCommandEvent& event ){

    wxFileDialog dlg (
		this,
		_("Выберите файл для добавления в библиотеку…"),
		wxEmptyString,
		wxEmptyString,
		_("Fiction books (*.fb2)|*.fb2"),
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		m_BooksInfoPanel->SetPage(blank_page);
		m_html.Empty();

		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			wxString filename = paths[i];
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
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		m_BooksInfoPanel->SetPage(blank_page);
		m_html.Empty();

		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			wxString filename = paths[i];
			wxString html;
			FbManager parser;
			parser.ParseZip(filename, html);
			m_BooksInfoPanel->AppendToPage(html);
		}
	}
}

void MyRuLibMainFrame::OnRegZip( wxCommandEvent& event ){

    wxFileDialog dlg (
		this,
		_("Выберите zip-файл для регистрации в библиотеке…"),
		wxEmptyString,
		wxEmptyString,
		_("Файлы Lib.rus.ec (*.zip)|???-??????-??????.zip"),
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			wxString filename = paths[i];
			wxString html;
			FbManager parser;
			parser.RegisterZip(filename);
		}
	}
}

void MyRuLibMainFrame::OnProgressStart(wxCommandEvent& event)
{
	m_StatusText = event.GetString();
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
	m_StatusText = wxEmptyString;
	m_ProgressBar->SetProgress(0);
	m_ProgressBar->SetStatusText(wxEmptyString, 0);
	m_ProgressBar->SetStatusText(wxEmptyString, 2);
}

void MyRuLibMainFrame::OnImageClick(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		int image = m_BooksListView->GetItemImage(item);
		image = ( image == 1 ? 0 : 1);
		if (m_BooksListView->GetItemBold(item)) {
			m_BooksListView->SetItemImage(item, image);
			wxTreeItemIdValue cookie;
			item = m_BooksListView->GetFirstChild(item, cookie);
			while (item.IsOk()) {
				m_BooksListView->SetItemImage(item, image);
				item = m_BooksListView->GetNextSibling (item);
			}
		} else {
			m_BooksListView->SetItemImage(item, image);
			wxTreeItemId parent = m_BooksListView->GetItemParent(item);
			wxTreeItemIdValue cookie;
			item = m_BooksListView->GetFirstChild(parent, cookie);
			while (item.IsOk()) {
				if (image != m_BooksListView->GetItemImage(item)) {
					image = 2;
					break;
				}
				item = m_BooksListView->GetNextSibling (item);
			}
			m_BooksListView->SetItemImage(parent, image);
		}
	}
	event.Veto();
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

void MyRuLibMainFrame::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data)
            FbManager::OpenBook(data->GetId());
	}
	event.Skip();
}

void MyRuLibMainFrame::OnSetAnnotation(wxCommandEvent& event)
{
    if (m_id_book == event.GetInt()) m_BooksInfoPanel->AppendToPage(event.GetString());
}
