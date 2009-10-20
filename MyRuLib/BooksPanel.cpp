#include "BooksPanel.h"
#include "FbConst.h"
#include "FbManager.h"
#include "FbFrameHtml.h"
#include "InfoCash.h"
#include "FbBookMenu.h"
#include "FbBookEvent.h"
#include "MyRuLibApp.h"
#include "FbDownloader.h"

BEGIN_EVENT_TABLE(BooksPanel, wxSplitterWindow)
    EVT_MENU(ID_BOOKINFO_UPDATE, BooksPanel::OnInfoUpdate)
    EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListActivated)
	EVT_TREE_ITEM_COLLAPSING(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListCollapsing)
	EVT_TREE_KEY_DOWN(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListKeyDown)
	EVT_TREE_STATE_IMAGE_CLICK(ID_BOOKS_LISTCTRL, BooksPanel::OnImageClick)
    EVT_TREE_ITEM_MENU(ID_BOOKS_LISTCTRL, BooksPanel::OnContextMenu)
	EVT_MENU(wxID_SELECTALL, BooksPanel::OnSelectAll)
	EVT_MENU(ID_UNSELECTALL, BooksPanel::OnUnselectAll)
	EVT_MENU(ID_OPEN_BOOK, BooksPanel::OnOpenBook)
	EVT_MENU(ID_FAVORITES_ADD, BooksPanel::OnFavoritesAdd)
	EVT_MENU(ID_EDIT_COMMENTS, BooksPanel::OnEditComments)
	EVT_MENU(ID_DOWNLOAD_BOOK, BooksPanel::OnDownloadBook)
END_EVENT_TABLE()

BooksPanel::BooksPanel()
    :wxSplitterWindow(), m_BookInfo(NULL), m_folder(fbNO_FOLDER)
{
}

bool BooksPanel::Create(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode)
{
    bool res = wxSplitterWindow::Create(parent, wxID_ANY, wxDefaultPosition, size, wxSP_NOBORDER, wxT("bookspanel"));
    if (res) {
        SetMinimumPaneSize(50);
        SetSashGravity(0.5);
        m_BookList = new FbBookList(this, ID_BOOKS_LISTCTRL, style);
        CreateBookInfo( (bool) FbParams::GetValue(keyType) );
		CreateColumns( (bool) FbParams::GetValue(keyMode) ? FB2_MODE_TREE : FB2_MODE_LIST );
    }
    return res;
}

void BooksPanel::CreateBookInfo(bool bVertical)
{
	if (m_BookInfo) Unsplit(m_BookInfo);

	m_BookInfo = new wxHtmlWindow(this, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BookInfo->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	if (bVertical)
		SplitVertically(m_BookList, m_BookInfo, GetSize().GetWidth()/2);
	else
		SplitHorizontally(m_BookList, m_BookInfo, GetSize().GetHeight()/2);

    BookTreeItemData * book = GetSelectedBook();
    if (!book) {
        m_BookInfo->SetPage(wxEmptyString);
    } else {
        InfoCash::UpdateInfo(this, book->GetId(), bVertical);
    }
}

BookTreeItemData * BooksPanel::GetSelectedBook()
{
	wxTreeItemId selected = m_BookList->GetSelection();
	if (selected.IsOk()) {
		return (BookTreeItemData*) m_BookList->GetItemData(selected);
    } else
        return NULL;
}

void BooksPanel::OnBooksListViewSelected(wxTreeEvent & event)
{
    m_BookInfo->SetPage(wxEmptyString);

	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*) m_BookList->GetItemData(selected);
		if (data) InfoCash::UpdateInfo(this, data->GetId(), GetSplitMode() == wxSPLIT_VERTICAL);
	}
}

void BooksPanel::OnBooksListCollapsing(wxTreeEvent & event)
{
	event.Veto();
}

void BooksPanel::OnImageClick(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		int image = m_BookList->GetItemImage(item);
		image = ( image == 1 ? 0 : 1);
		if (m_BookList->GetItemBold(item)) {
			m_BookList->SetItemImage(item, image);
			wxTreeItemIdValue cookie;
			item = m_BookList->GetFirstChild(item, cookie);
			while (item.IsOk()) {
				wxTreeItemIdValue cookie;
				wxTreeItemId subitem = m_BookList->GetFirstChild(item, cookie);
				while (subitem.IsOk()) {
					m_BookList->SetItemImage(subitem, image);
					subitem = m_BookList->GetNextSibling(subitem);
				}
				m_BookList->SetItemImage(item, image);
				item = m_BookList->GetNextSibling (item);
			}
		} else {
			m_BookList->SetItemImage(item, image);
			wxTreeItemId parent = m_BookList->GetItemParent(item);
			wxTreeItemIdValue cookie;
			item = m_BookList->GetFirstChild(parent, cookie);
			while (item.IsOk()) {
				if (image != m_BookList->GetItemImage(item)) {
					image = 2;
					break;
				}
				item = m_BookList->GetNextSibling (item);
			}
			m_BookList->SetItemImage(parent, image);
		}
	}
	event.Veto();
}

void BooksPanel::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BookList->GetItemData(selected);
		if (data) FbManager::OpenBook(data->GetId(), data->file_type);
	}
}

void BooksPanel::OnBooksListKeyDown(wxTreeEvent & event)
{
	if (event.GetKeyCode() == 0x20) {
		wxArrayTreeItemIds selections;
		size_t count = m_BookList->GetSelections(selections);
		int image = 0;
		for (size_t i=0; i<count; ++i) {
            wxTreeItemId selected = selections[i];
		    if (i==0)
                image = (m_BookList->GetItemImage(selected) + 1) % 2;
			m_BookList->SetItemImage(selected, image);
		}
		event.Veto();
	} else
		event.Skip();
}

void BooksPanel::OnInfoUpdate(wxCommandEvent& event)
{
	wxTreeItemId selected = m_BookList->GetSelection();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BookList->GetItemData(selected);
		if (data && (data->GetId() == event.GetInt())) {
			wxString html = event.GetString();
			m_BookInfo->SetPage(html);
		}
	}
}

void BooksPanel::OnSubmenu(wxCommandEvent& event)
{
    wxPostEvent(m_BookList, event);
}

void BooksPanel::OnContextMenu(wxTreeEvent& event)
{
    wxPoint point = event.GetPoint();
    // If from keyboard
    if (point.x == -1 && point.y == -1) {
        wxSize size = GetSize();
        point.x = size.x / 3;
        point.y = size.y / 3;
    }
    ShowContextMenu(point, event.GetItem());
}

void BooksPanel::ShowContextMenu(const wxPoint& pos, wxTreeItemId item)
{
	int id = 0;
	if (item.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BookList->GetItemData(item);
		if (data) id = data->GetId();
	}
    FbBookMenu menu(id, m_folder);
	menu.ConnectFolders(this, wxCommandEventHandler(BooksPanel::OnFolderAdd));
	menu.ConnectAuthors(this, wxCommandEventHandler(BooksPanel::OnOpenAuthor));
    PopupMenu(&menu, pos.x, pos.y);
}

void BooksPanel::OnSelectAll(wxCommandEvent& event)
{
    m_BookList->SelectAll();
}

void BooksPanel::OnUnselectAll(wxCommandEvent& event)
{
    m_BookList->SelectAll(0);
}

void BooksPanel::OnOpenBook(wxCommandEvent & event)
{
    BookTreeItemData * data = GetSelectedBook();
    if (data) FbManager::OpenBook(data->GetId(), data->file_type);
}

void BooksPanel::OnDownloadBook(wxCommandEvent & event)
{
    BookTreeItemData * data = GetSelectedBook();
    if (data) {
//    	FbDownloadThread::Execute();
	}
}

class FbAppendFavouritesThread: public wxThread
{
    public:
        FbAppendFavouritesThread(wxString selections, int folder = 0): m_selections(selections), m_folder(folder) {};
        void * Entry();
    private:
        wxString m_selections;
        int m_folder;
        FbCommonDatabase m_database;
};

void * FbAppendFavouritesThread::Entry()
{
    m_database.AttachConfig();
    wxString sql = wxString::Format(wxT("INSERT INTO favorites(id_folder,md5sum) SELECT DISTINCT %d, md5sum FROM books WHERE id IN (%s)"), m_folder, m_selections.c_str());
    m_database.ExecuteUpdate(sql);

	wxCommandEvent event(fbEVT_BOOK_ACTION, ID_UPDATE_FOLDER);
    event.SetInt(m_folder);
    wxPostEvent(wxGetApp().GetTopWindow(), event);

    return NULL;
}

void BooksPanel::OnFavoritesAdd(wxCommandEvent & event)
{
    wxThread * thread = new FbAppendFavouritesThread( m_BookList->GetSelected() );
    if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void BooksPanel::OnFolderAdd(wxCommandEvent& event)
{
	int folder = FbBookMenu::GetFolder(event.GetId());
    wxThread * thread = new FbAppendFavouritesThread( m_BookList->GetSelected(), folder );
    if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void BooksPanel::OnEditComments(wxCommandEvent & event)
{
	wxTreeItemId selected = m_BookList->GetSelection();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BookList->GetItemData(selected);
		if (data) new FbFrameHtml((wxAuiMDIParentFrame*)wxGetApp().GetTopWindow(), *data);
	}
}

void BooksPanel::OnOpenAuthor(wxCommandEvent& event)
{
	int author = FbBookMenu::GetAuthor(event.GetId());
	if (author == 0) return;

	wxCommandEvent subevent(fbEVT_BOOK_ACTION, ID_OPEN_AUTHOR);
    subevent.SetInt(author);
    wxPostEvent(wxGetApp().GetTopWindow(), subevent);
}

void BooksPanel::EmptyBooks(const wxString title)
{
	m_AuthorItem = 0;
	m_SequenceItem = 0;

    BookListUpdater updater(m_BookList);
	wxTreeItemId root = m_BookList->AddRoot(title);
	m_BookInfo->SetPage(wxEmptyString);
}

void BooksPanel::AppendAuthor(const wxString title)
{
	FbTreeListUpdater updater(m_BookList);
	wxTreeItemId parent = m_BookList->GetRootItem();
	m_AuthorItem = m_BookList->AppendItem(parent, title, 0);
	m_BookList->SetItemBold(m_AuthorItem, true);
	m_BookList->Expand(parent);
}

void BooksPanel::AppendSequence(const wxString title)
{
	wxString text = title.IsEmpty() ? strOtherSequence : title;
	FbTreeListUpdater updater(m_BookList);
	wxTreeItemId parent = m_AuthorItem.IsOk() ? m_AuthorItem : m_BookList->GetRootItem();
	m_SequenceItem = m_BookList->AppendItem(parent, text, 0);
	m_BookList->SetItemBold(m_SequenceItem, true);
	m_BookList->Expand(parent);
}

wxString BooksPanel::F(const int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return F(hi) + wxT(" ") + wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

void BooksPanel::AppendBook(BookTreeItemData * data, const wxString & authors)
{
	FbTreeListUpdater updater(m_BookList);

	wxString file_type = data->file_type + wxT(" ");
	wxString file_size = F(data->file_size/1024) + wxT(" ");
	wxTreeItemId parent;

    switch (m_ListMode) {
        case FB2_MODE_TREE: {
			parent = m_SequenceItem.IsOk() ? m_SequenceItem : ( m_AuthorItem.IsOk() ? m_AuthorItem : m_BookList->GetRootItem() );
			wxTreeItemId item = m_BookList->AppendItem(parent, data->title, 0, -1, data);
			if (data->number) m_BookList->SetItemText(item, 1, wxString::Format(wxT(" %d "), data->number));
			m_BookList->SetItemText(item, 2, file_type);
			m_BookList->SetItemText(item, 3, file_size);
        } break;
        case FB2_MODE_LIST: {
			parent = m_BookList->GetRootItem();
			wxTreeItemId item = m_BookList->AppendItem(parent, data->title, 0, -1, data);
			m_BookList->SetItemText(item, 1, authors);
			m_BookList->SetItemText(item, 2, file_type);
			m_BookList->SetItemText(item, 3, file_size);
        } break;
    }
	m_BookList->Expand(parent);
}

void BooksPanel::CreateColumns(FbListMode mode)
{
    m_ListMode = mode;

	m_AuthorItem = 0;
	m_SequenceItem = 0;

    BookListUpdater updater(m_BookList);

    m_BookList->EmptyCols();

    switch (m_ListMode) {
        case FB2_MODE_TREE: {
            m_BookList->AddColumn (_("Заголовок"), 13, wxALIGN_LEFT);
            m_BookList->AddColumn (_("№"), 2, wxALIGN_RIGHT);
            m_BookList->AddColumn (_("Тип"), 2, wxALIGN_RIGHT);
            m_BookList->AddColumn (_("Размер, Кб"), 3, wxALIGN_RIGHT);
        } break;
        case FB2_MODE_LIST: {
            m_BookList->AddColumn (_("Заголовок"), 9, wxALIGN_LEFT);
            m_BookList->AddColumn (_("Автор"), 6, wxALIGN_LEFT);
            m_BookList->AddColumn (_("Тип"), 2, wxALIGN_RIGHT);
            m_BookList->AddColumn (_("Размер, Кб"), 3, wxALIGN_RIGHT);
        } break;
    }

	wxTreeItemId root = m_BookList->AddRoot(wxEmptyString);
	m_BookInfo->SetPage(wxEmptyString);
}

