/***************************************************************
 * Name:      MyRuLibMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/artprov.h>
#include <wx/splitter.h>
#include <wx/imaglist.h>
#include "MyRuLibMain.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "db/Books.h"
#include "db/Params.h"
#include "FbManager.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "SettingsDlg.h"
#include "InfoThread.h"
#include "InfoCash.h"
#include "ExternalDlg.h"

#include "XpmBitmaps.h"

#define ID_LETTER_RU 30100
#define ID_LETTER_EN 30200

wxString alphabetRu = _("АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
wxString alphabetEn = wxT("#ABCDEFGHIJKLMNOPQRSTUVWXWZ");
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
    EVT_TOOL(ID_NEW_ZIP, MyRuLibMainFrame::OnNewZip)
    EVT_TOOL(ID_REG_ZIP, MyRuLibMainFrame::OnRegZip)
    EVT_MENU(ID_PROGRESS_START, MyRuLibMainFrame::OnProgressStart)
    EVT_MENU(ID_PROGRESS_UPDATE, MyRuLibMainFrame::OnProgressUpdate)
    EVT_MENU(ID_PROGRESS_FINISH, MyRuLibMainFrame::OnProgressFinish)
    EVT_MENU(ID_FB2_ONLY, MyRuLibMainFrame::OnChangeFilter)
    EVT_MENU(ID_EXTERNAL, MyRuLibMainFrame::OnExternal)
    EVT_MENU(ID_BOOKINFO_UPDATE, MyRuLibMainFrame::OnInfoUpdate)
    EVT_MENU(ID_ERROR, MyRuLibMainFrame::OnError)
END_EVENT_TABLE()

MyRuLibMainFrame::MyRuLibMainFrame()
	:m_BooksInfoPanel(NULL)
{
	Create(NULL, wxID_ANY, _("MyRuLib - My Russian Library"));
}

bool MyRuLibMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title)
{
	bool res = wxFrame::Create(parent, id, title, wxDefaultPosition, wxSize(700, 500));
	if(res)	{
		CreateControls();
        #if defined(__WXMSW__)
		wxIcon icon(wxT("aaaa"));
		SetIcon(icon);
        #else
		SetIcon(wxArtProvider::GetIcon(wxART_FRAME_ICON));
        #endif
	}
	return res;
}

void MyRuLibMainFrame::CreateControls()
{
	wxMenuBar * menuBar = new wxMenuBar;

	wxMenu * fileMenu = new wxMenu;
	wxMenuItem * tempItem = fileMenu->Append(wxID_ANY, wxT("Непонятная ошибка с картинками в меню"));
	tempItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FIND));
	fileMenu->Append(ID_NEW_ZIP, _("Добавить файл ZIP…"))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	fileMenu->Append(ID_REG_ZIP, _("Зарегистрировать ZIP…"))->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_BOOK));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_EXTERNAL, _("Записать на устройство"))->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_DIR_UP));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, _("Выход\tAlt+F4"))->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT));
	fileMenu->Delete(tempItem);
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

	SetToolBar(m_toolBar = CreateButtonBar());

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(CreateAlphaBar(alphabetRu, ID_LETTER_RU), 0, wxEXPAND, 5);
	sizer->Add(CreateAlphaBar(alphabetEn, ID_LETTER_EN), 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_AuthorsListBox = new wxListBox(splitter, ID_AUTHORS_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, NULL, wxSUNKEN_BORDER);

	m_books_splitter = new wxSplitterWindow(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	m_books_splitter->SetMinimumPaneSize(50);
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
    m_BooksListView->colSizes.Add(9);
    m_BooksListView->colSizes.Add(1);
    m_BooksListView->colSizes.Add(4);
    m_BooksListView->colSizes.Add(2);

    wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	images->Add (wxBitmap(checkout_xpm));
	m_BooksListView->AssignImageList (images);

	splitter->SplitVertically(m_AuthorsListBox, m_books_splitter, 160);

    vertical = FbParams().GetValue(FB_VIEW_TYPE);
	CreateBookInfo();

    wxDateTime now = wxDateTime::Now();
    int random = now.GetHour() * 60 * 60 + now.GetMinute() * 60 + now.GetSecond();
	random = random % alphabetRu.Len();

	FbManager::FillAuthorsChar(m_AuthorsListBox, alphabetRu[random]);

	const int widths[] = {-92, -57, -35, -22};
    m_ProgressBar = new ProgressBar(this, ID_PROGRESSBAR);
    m_ProgressBar->SetFieldsCount(4);
	m_ProgressBar->SetStatusWidths(4, widths);
	SetStatusBar(m_ProgressBar);

	Centre();
}

void MyRuLibMainFrame::CreateBookInfo()
{
	if (m_BooksInfoPanel) m_books_splitter->Unsplit(m_BooksInfoPanel);

	m_BooksInfoPanel = new wxHtmlWindow(m_books_splitter, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BooksInfoPanel->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	if (vertical)
		m_books_splitter->SplitVertically(m_BooksListView, m_BooksInfoPanel, m_books_splitter->GetSize().GetWidth()/2);
	else
		m_books_splitter->SplitHorizontally(m_BooksListView, m_BooksInfoPanel, m_books_splitter->GetSize().GetHeight()/2);

    wxString html = InfoCash::GetInfo(GetSelectedBook(), vertical);
    m_BooksInfoPanel->SetPage(html);
    InfoCash::ShowInfo(this, GetSelectedBook());
}

void MyRuLibMainFrame::OnSetup(wxCommandEvent & event)
{
    SettingsDlg::Execute(this);
}

void MyRuLibMainFrame::OnChangeView(wxCommandEvent & event)
{
	vertical = (event.GetId() == ID_SPLIT_HORIZONTAL);
	FbParams().SetValue(FB_VIEW_TYPE, vertical);
	CreateBookInfo();
}

void MyRuLibMainFrame::OnAbout(wxCommandEvent & event)
{
    wxMessageBox(_T("MyRuLib - version 0.01 (alpha)\n\nhttp://www.lintest.ru\nmail@lintest.ru"));
}

wxToolBar * MyRuLibMainFrame::CreateButtonBar()
{

	wxToolBar * toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT);
	toolBar->AddTool(ID_NEW_ZIP, _("Импорт"), wxArtProvider::GetBitmap(wxART_NEW), _("Добавить в библиотеку новые файлы ZIP"));
	toolBar->AddSeparator();
	m_FindTextCtrl = new wxTextCtrl( toolBar, ID_FIND_TEXT, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER );
	toolBar->AddControl( m_FindTextCtrl );
	toolBar->AddTool(ID_FIND_BTN, _("Поиск"), wxArtProvider::GetBitmap(wxART_FIND), _("Поиск по подстроке"));
	toolBar->AddSeparator();
	toolBar->AddTool(ID_EXTERNAL, _("Экспорт"), wxArtProvider::GetBitmap(wxART_GO_DIR_UP), _("Запись на внешнее устройство"));
	toolBar->AddSeparator();
	toolBar->AddTool(ID_FB2_ONLY, _("Фильтр"), wxArtProvider::GetBitmap(wxART_HELP_BOOK), _("Только файлы Fb2"), wxITEM_CHECK);
	toolBar->ToggleTool(ID_FB2_ONLY, FbParams().GetValue(FB_FB2_ONLY) );
	toolBar->Realize();

	return toolBar;
}

void MyRuLibMainFrame::OnChangeFilter(wxCommandEvent& event)
{
    FbParams().SetValue(FB_FB2_ONLY, m_toolBar-> GetToolState(ID_FB2_ONLY));

    RecordIDClientData * data = (RecordIDClientData *)
        m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
    if(data) {
        FbManager::FillBooks(m_BooksListView, data->GetID(), m_toolBar-> GetToolState(ID_FB2_ONLY));
        m_BooksInfoPanel->SetPage(wxEmptyString);
    }
}

wxToolBar * MyRuLibMainFrame::CreateAlphaBar(const wxString & alphabet, int toolid)
{
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

void MyRuLibMainFrame::OnExit(wxCommandEvent & event)
{
	wxUnusedVar(event);
	Close();
}

void MyRuLibMainFrame::OnAuthorsListBoxSelected(wxCommandEvent & event)
{
	RecordIDClientData * data = (RecordIDClientData *)event.GetClientObject();
	if(data) {
		FbManager::FillBooks(m_BooksListView, data->GetID(), m_toolBar-> GetToolState(ID_FB2_ONLY));
		m_BooksInfoPanel->SetPage(wxEmptyString);
	}
}

void MyRuLibMainFrame::OnBooksListViewSelected(wxTreeEvent & event)
{
    m_BooksInfoPanel->SetPage(wxEmptyString);

	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data) InfoCash::ShowInfo(this, data->GetId());
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
			FbManager::FillBooks(m_BooksListView, data->GetID(), m_toolBar-> GetToolState(ID_FB2_ONLY));
			m_BooksInfoPanel->SetPage(wxEmptyString);
		}
	} else {
		m_BooksListView->DeleteRoot();
		m_BooksInfoPanel->SetPage(wxEmptyString);
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
		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			FbManager::ParseZip(paths[i]);
		}
	}
}

void MyRuLibMainFrame::OnRegZip( wxCommandEvent& event ){

    wxFileDialog dlg (
		this,
		_("Выберите zip-файл для регистрации в библиотеке…"),
		wxEmptyString,
		wxEmptyString,
		_("Файлы Lib.rus.ec (*.zip)|*.zip"),
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths(paths);
		for (size_t i = 0; i < paths.GetCount(); ++i) {
			FbManager::RegisterZip(paths[i]);
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
		BookTreeItemData * data = (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data) FbManager::OpenBook(data->GetId());
	}
	event.Skip();
}

void MyRuLibMainFrame::OnInfoUpdate(wxCommandEvent& event)
{
	wxTreeItemId selected = m_BooksListView->GetSelection();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data && (data->GetId() == event.GetInt())) {
            wxString html = InfoCash::GetInfo(event.GetInt(), vertical);
            m_BooksInfoPanel->SetPage(html);
		}
	}
	event.Skip();
}

int MyRuLibMainFrame::GetSelectedBook()
{
	wxTreeItemId selected = m_BooksListView->GetSelection();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		return data->GetId();
    } else
        return 0;
}

void MyRuLibMainFrame::OnExternal(wxCommandEvent& event)
{
    int sel = m_AuthorsListBox->GetSelection();

    if (sel == wxNOT_FOUND)
        wxMessageBox(wxT("Не выбран ни один автор."));
    else
        ExternalDlg::Execute(this, m_BooksListView, m_AuthorsListBox->GetString(sel));

	event.Skip();
}

void MyRuLibMainFrame::OnError(wxCommandEvent& event)
{
	event.Skip();
    wxLogError(event.GetString());
}
