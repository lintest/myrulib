#include "BooksPanel.h"
#include "FbConst.h"
#include "FbParams.h"
#include "InfoCash.h"
#include "MyRuLibApp.h"

BEGIN_EVENT_TABLE(BooksPanel, wxSplitterWindow)
	EVT_MENU(ID_SPLIT_HORIZONTAL, BooksPanel::OnChangeView)
	EVT_MENU(ID_SPLIT_VERTICAL, BooksPanel::OnChangeView)
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
END_EVENT_TABLE()

BooksPanel::BooksPanel()
    :wxSplitterWindow(), m_BookInfo(NULL)
{
}

BooksPanel::BooksPanel(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, long substyle, const wxString& name)
    :wxSplitterWindow(parent, id, pos, size, style, wxT("bookspanel")), m_BookInfo(NULL)
{
    Create(parent, id, pos, size, style, substyle, name);
}

bool BooksPanel::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, long substyle, const wxString& name)
{
    bool res = wxSplitterWindow::Create(parent, id, pos, size, style, wxT("bookspanel"));
    if (res) {
        SetMinimumPaneSize(50);
        SetSashGravity(0.5);
        m_BookList = new BookListCtrl(this, ID_BOOKS_LISTCTRL, substyle);
        CreateBookInfo();
    }
    return res;
}

void BooksPanel::CreateBookInfo()
{
    int vertical = FbParams().GetValue(FB_VIEW_TYPE);

	if (m_BookInfo) Unsplit(m_BookInfo);

	m_BookInfo = new wxHtmlWindow(this, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BookInfo->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	if (vertical)
		SplitVertically(m_BookList, m_BookInfo, GetSize().GetWidth()/2);
	else
		SplitHorizontally(m_BookList, m_BookInfo, GetSize().GetHeight()/2);

    BookTreeItemData * book = GetSelectedBook();
    if (!book) {
        m_BookInfo->SetPage(wxEmptyString);
    } else {
        wxString html = InfoCash::GetInfo(book->GetId(), vertical);
        m_BookInfo->SetPage(html);
        InfoCash::ShowInfo(this, book->GetId(), book->file_type);
    }
}

void BooksPanel::CreateAuthorColumns()
{
    m_BookList->AddColumn (_T("Заголовок"), 9, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("№"), 1, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("Имя файла"), 4, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("Размер, Кб"), 2, wxALIGN_RIGHT);
}

void BooksPanel::CreateSearchColumns()
{
    m_BookList->AddColumn (_T("Заголовок"), 9, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("Автор"), 6, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("№"), 1, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("Имя файла"), 4, wxALIGN_LEFT);
    m_BookList->AddColumn (_T("Размер, Кб"), 2, wxALIGN_RIGHT);
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
		if (data) InfoCash::ShowInfo(this, data->GetId(), data->file_type);
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

void BooksPanel::OnChangeView(wxCommandEvent & event)
{
	int vertical = (event.GetId() == ID_SPLIT_VERTICAL);
	FbParams().SetValue(FB_VIEW_TYPE, vertical);
	CreateBookInfo();
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
            wxString html = InfoCash::GetInfo(event.GetInt(), GetSplitMode() == wxSPLIT_VERTICAL);
            m_BookInfo->SetPage(html);
		}
	}
}

void BooksPanel::FillByAuthor(int id_author)
{
	wxString sql = wxT("\
        SELECT books.id, books.title, books.file_size, books.file_type, books.file_name, books.id_author, sequences.value AS sequence, bookseq.number\
        FROM books \
            LEFT JOIN bookseq ON bookseq.id_book=books.id AND bookseq.id_author = books.id_author \
            LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
        WHERE books.id_author = ? \
        ORDER BY sequences.value, bookseq.number, books.title \
    ");

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    wxSQLite3Statement stmt = wxGetApp().GetDatabase().PrepareStatement(sql);
    stmt.Bind(1, id_author);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

    wxString thisSequence;
    wxTreeItemId parent;

	m_BookList->Freeze();
    m_BookList->DeleteRoot();

    wxTreeItemId root = m_BookList->AddRoot(wxT("root"));
    m_BookList->SetItemBold(root, true);

    while (result.NextRow()) {
	    wxString nextSequence = result.GetString(wxT("sequence"));
	    if (thisSequence != nextSequence || !parent.IsOk()) {
	        thisSequence = nextSequence;
            parent = m_BookList->AppendItem(root, thisSequence.IsEmpty() ? strOtherSequence : thisSequence );
            m_BookList->SetItemBold(parent, true);
	    }
	    BookTreeItemData * data = new BookTreeItemData(result);
        wxTreeItemId item = m_BookList->AppendItem(parent, data->title, 0, -1, data);
        m_BookList->SetItemText (item, 1, wxString::Format(wxT("%d"), data->number));
        m_BookList->SetItemText (item, 2, data->file_name);
        m_BookList->SetItemText (item, 3, wxString::Format(wxT("%d "), data->file_size/1024));
	}

    m_BookList->ExpandAll( m_BookList->GetRootItem() );
	m_BookList->Thaw();

	m_BookInfo->SetPage(wxEmptyString);
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
    ShowContextMenu(point);
}

void BooksPanel::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;

	menu.Append(ID_OPEN_BOOK, _("Открыть книгу\tEnter"));
    menu.AppendSeparator();
	menu.Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	menu.Append(ID_UNSELECTALL, _("Отменить выделение"));
    menu.AppendSeparator();
    menu.Append(ID_FAVORITES_ADD, _T("Добавить в избранное"));

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

void BooksPanel::OnFavoritesAdd(wxCommandEvent & event)
{
    wxMessageBox(_("Функционал не реализован в данной версии."));
}
