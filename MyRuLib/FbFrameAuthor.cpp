#include "FbFrameAuthor.h"
#include <wx/artprov.h>
#include "ExternalDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "XpmBitmaps.h"
#include "RecordIDClientData.h"

BEGIN_EVENT_TABLE(FbFrameAuthor, wxAuiMDIChildFrame)
	EVT_MENU(ID_SPLIT_HORIZONTAL, FbFrameAuthor::OnChangeView)
	EVT_MENU(ID_SPLIT_VERTICAL, FbFrameAuthor::OnChangeView)
    EVT_UPDATE_UI(ID_SPLIT_HORIZONTAL, FbFrameAuthor::OnChangeViewUpdateUI)
    EVT_UPDATE_UI(ID_SPLIT_VERTICAL, FbFrameAuthor::OnChangeViewUpdateUI)
//  EVT_MENU(ID_FB2_ONLY, FbFrameAuthor::OnChangeFilter)
	EVT_MENU(wxID_SELECTALL, FbFrameAuthor::OnSelectAll)
    EVT_MENU(wxID_SAVE, FbFrameAuthor::OnExternal)
    EVT_LISTBOX(ID_AUTHORS_LISTBOX, FbFrameAuthor::OnAuthorsListBoxSelected)
    EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, FbFrameAuthor::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, FbFrameAuthor::OnBooksListActivated)
	EVT_TREE_ITEM_COLLAPSING(ID_BOOKS_LISTCTRL, FbFrameAuthor::OnBooksListCollapsing)
	EVT_TREE_KEY_DOWN(ID_BOOKS_LISTCTRL, FbFrameAuthor::OnBooksListKeyDown)
	EVT_TREE_STATE_IMAGE_CLICK(ID_BOOKS_LISTCTRL, FbFrameAuthor::OnImageClick)
    EVT_TEXT_ENTER(ID_FIND_TEXT, FbFrameAuthor::OnFindTextEnter)
    EVT_MENU(ID_BOOKINFO_UPDATE, FbFrameAuthor::OnInfoUpdate)
END_EVENT_TABLE()

FbFrameAuthor::FbFrameAuthor()
	:m_BooksInfoPanel(NULL)
{
}

FbFrameAuthor::FbFrameAuthor(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
	:m_BooksInfoPanel(NULL)
{
	Create(parent, id, title);
}

bool FbFrameAuthor::Create(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	bool res = wxAuiMDIChildFrame::Create(parent, id, title);
	if (res) CreateControls();
	return res;
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

	m_AuthorsListBox = new wxListBox(splitter, ID_AUTHORS_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, NULL, wxSUNKEN_BORDER);

	m_BooksSplitter = new wxSplitterWindow(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	m_BooksSplitter->SetMinimumPaneSize(50);
	m_BooksSplitter->SetSashGravity(0.5);

	long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksListView = new BookListCtrl(m_BooksSplitter, ID_BOOKS_LISTCTRL, style);
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

	splitter->SplitVertically(m_AuthorsListBox, m_BooksSplitter, 160);

	CreateBookInfo();

    wxDateTime now = wxDateTime::Now();
    int random = now.GetHour() * 60 * 60 + now.GetMinute() * 60 + now.GetSecond();
	random = random % alphabetRu.Len();

	FbManager::FillAuthorsChar(m_AuthorsListBox, alphabetRu[random]);
	m_RuAlphabar->ToggleTool(ID_LETTER_RU + random, true );

	SetMenuBar(CreateMenuBar());
}

wxMenuBar * FbFrameAuthor::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;
	wxMenuItem * tempItem;
	wxMenu * menu;

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_NEW, _("Добавить файл"))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_OPEN, _("Добавить директорию"))->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN));
	menu->AppendSeparator();
	menu->Append(wxID_SAVE, _("Записать на устройство"))->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE));
	menu->AppendSeparator();
	menu->Append(wxID_EXIT, _("Выход\tAlt+F4"))->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Файл"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Правка"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_ANY, _("Параметры базы данных"));
	menu->Append(wxID_ANY, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Сервис"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->AppendCheckItem(ID_SPLIT_HORIZONTAL, _("&Просмотр справа"));
	menu->AppendCheckItem(ID_SPLIT_VERTICAL, _("&Просмтр снизу"));
	menu->AppendSeparator();
	menu->Append(ID_LOG_TEXTCTRL, _("Скрыть окно сообщений\tCtrl+Z"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Вид"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_ABOUT, _("О программе…"))->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_PAGE));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&?"));

	return menuBar;
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

void FbFrameAuthor::CreateBookInfo()
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

    BookTreeItemData * book = GetSelectedBook();
    if (!book) {
        m_BooksInfoPanel->SetPage(wxEmptyString);
    } else {
        wxString html = InfoCash::GetInfo(book->GetId(), vertical);
        m_BooksInfoPanel->SetPage(html);
        InfoCash::ShowInfo(this, book->GetId(), book->file_type);
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

	FbManager::FillAuthorsChar(m_AuthorsListBox, alphabet[position]);
	SelectFirstAuthor();
}

BookTreeItemData * FbFrameAuthor::GetSelectedBook()
{
	wxTreeItemId selected = m_BooksListView->GetSelection();
	if (selected.IsOk()) {
		return (BookTreeItemData*)m_BooksListView->GetItemData(selected);
    } else
        return NULL;
}

void FbFrameAuthor::SelectFirstAuthor()
{
	if(m_AuthorsListBox->GetCount()) {
		m_AuthorsListBox->SetSelection(0);
		RecordIDClientData * data = (RecordIDClientData *)
			m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
		if(data) {
			FbManager::FillBooks(m_BooksListView, data->GetID(), false);
			m_BooksInfoPanel->SetPage(wxEmptyString);
		}
	} else {
		m_BooksListView->DeleteRoot();
		m_BooksInfoPanel->SetPage(wxEmptyString);
	}
}

void FbFrameAuthor::OnAuthorsListBoxSelected(wxCommandEvent & event)
{
	RecordIDClientData * data = (RecordIDClientData *)event.GetClientObject();
	if(data) {
		FbManager::FillBooks(m_BooksListView, data->GetID(), false);
		m_BooksInfoPanel->SetPage(wxEmptyString);
	}
}

void FbFrameAuthor::OnBooksListViewSelected(wxTreeEvent & event)
{
    m_BooksInfoPanel->SetPage(wxEmptyString);

	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data) InfoCash::ShowInfo(this, data->GetId(), data->file_type);
	}
}

void FbFrameAuthor::OnBooksListCollapsing(wxTreeEvent & event)
{
	event.Veto();
}
void FbFrameAuthor::OnFindTextEnter( wxCommandEvent& event )
{
    wxString text = event.GetString();
	if (text.IsEmpty()) return;
    ToggleAlphabar(0);
    FbManager::FillAuthorsText(m_AuthorsListBox, text);
    SelectFirstAuthor();
}

void FbFrameAuthor::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data) FbManager::OpenBook(data->GetId(), data->file_type);
	}
}

void FbFrameAuthor::OnBooksListKeyDown(wxTreeEvent & event)
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

void FbFrameAuthor::OnInfoUpdate(wxCommandEvent& event)
{
    int vertical = FbParams().GetValue(FB_VIEW_TYPE);

	wxTreeItemId selected = m_BooksListView->GetSelection();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BooksListView->GetItemData(selected);
		if (data && (data->GetId() == event.GetInt())) {
            wxString html = InfoCash::GetInfo(event.GetInt(), vertical);
            m_BooksInfoPanel->SetPage(html);
		}
	}
}

void FbFrameAuthor::OnExternal(wxCommandEvent& event)
{
    int sel = m_AuthorsListBox->GetSelection();

    if (sel == wxNOT_FOUND)
        wxMessageBox(wxT("Не выбран ни один автор."));
    else
        ExternalDlg::Execute(this, m_BooksListView, m_AuthorsListBox->GetString(sel));
}

void FbFrameAuthor::OnImageClick(wxTreeEvent &event)
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
				wxTreeItemIdValue cookie;
				wxTreeItemId subitem = m_BooksListView->GetFirstChild(item, cookie);
				while (subitem.IsOk()) {
					m_BooksListView->SetItemImage(subitem, image);
					subitem = m_BooksListView->GetNextSibling(subitem);
				}
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

void FbFrameAuthor::OnSelectAll(wxCommandEvent& event)
{
    m_BooksListView->SelectAll();
}

void FbFrameAuthor::OnChangeView(wxCommandEvent & event)
{
	int vertical = (event.GetId() == ID_SPLIT_HORIZONTAL);
	FbParams().SetValue(FB_VIEW_TYPE, vertical);
	CreateBookInfo();
}

void FbFrameAuthor::OnChangeViewUpdateUI(wxUpdateUIEvent & event)
{
    int vertical = FbParams().GetValue(FB_VIEW_TYPE);

    if (event.GetId() == ID_SPLIT_HORIZONTAL)
        event.Check(vertical);
    else
        event.Check(!vertical);
}

