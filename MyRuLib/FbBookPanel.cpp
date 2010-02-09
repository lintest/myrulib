#include "FbBookPanel.h"
#include "FbConst.h"
#include "FbFrameHtml.h"
#include "InfoCash.h"
#include "FbBookMenu.h"
#include "MyRuLibApp.h"
#include "FbDownloader.h"
#include "FbUpdateThread.h"
#include "FbEditBook.h"
#include "ZipReader.h"
#include "FbHtmlWindow.h"

BEGIN_EVENT_TABLE(FbBookPanel, wxSplitterWindow)
	EVT_MENU(ID_BOOKINFO_UPDATE, FbBookPanel::OnInfoUpdate)
	EVT_COMMAND(ID_AUTHOR_INFO, fbEVT_BOOK_ACTION, FbBookPanel::OnAuthorInfo)
	EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, FbBookPanel::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, FbBookPanel::OnBooksListActivated)
	EVT_TREE_ITEM_MENU(ID_BOOKS_LISTCTRL, FbBookPanel::OnContextMenu)
	EVT_MENU(ID_SPLIT_HORIZONTAL, FbBookPanel::OnChangeView)
	EVT_MENU(ID_SPLIT_VERTICAL, FbBookPanel::OnChangeView)
	EVT_MENU(ID_SPLIT_NOTHING, FbBookPanel::OnChangeView)
	EVT_MENU(wxID_SELECTALL, FbBookPanel::OnSelectAll)
	EVT_MENU(ID_UNSELECTALL, FbBookPanel::OnUnselectAll)
	EVT_MENU(ID_OPEN_BOOK, FbBookPanel::OnOpenBook)
	EVT_MENU(ID_BOOK_PAGE, FbBookPanel::OnBookPage)
	EVT_MENU(ID_DOWNLOAD_BOOK, FbBookPanel::OnDownloadBook)
	EVT_MENU(ID_SYSTEM_DOWNLOAD, FbBookPanel::OnSystemDownload)
	EVT_MENU(ID_DELETE_DOWNLOAD, FbBookPanel::OnDeleteDownload)
	EVT_MENU(ID_FAVORITES_ADD, FbBookPanel::OnFavoritesAdd)
	EVT_MENU(ID_EDIT_COMMENTS, FbBookPanel::OnEditComments)
	EVT_MENU(ID_RATING_5, FbBookPanel::OnChangeRating)
	EVT_MENU(ID_RATING_4, FbBookPanel::OnChangeRating)
	EVT_MENU(ID_RATING_3, FbBookPanel::OnChangeRating)
	EVT_MENU(ID_RATING_2, FbBookPanel::OnChangeRating)
	EVT_MENU(ID_RATING_1, FbBookPanel::OnChangeRating)
	EVT_MENU(ID_RATING_0, FbBookPanel::OnChangeRating)
	EVT_MENU(wxID_EDIT, FbBookPanel::OnModifyBooks)
	EVT_MENU(wxID_DELETE, FbBookPanel::OnDeleteBooks)
END_EVENT_TABLE()

FbBookPanel::FbBookPanel(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode)
	: wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, size, wxSP_NOBORDER, wxT("bookspanel")),
		m_BookInfo(NULL), m_selected(0), m_KeyView(keyType), m_master(NULL)
{
	SetMinimumPaneSize(50);
	SetSashGravity(0.5);
	m_BookList = new FbBookList(this, ID_BOOKS_LISTCTRL, style);
	m_BookInfo = new FbHtmlWindow(this, ID_BOOKS_INFO_PANEL);

	int mode = FbParams::GetValue(keyType);
	if (mode == FB2_VIEW_NOTHING) {
		Initialize(m_BookList);
		m_BookInfo->Show(false);
	} else {
		SetViewMode(mode);
	}

	{
		BookListUpdater updater(m_BookList);
		m_BookList->AddColumn (_("Заголовок"), 10, wxALIGN_LEFT);
		m_BookList->AddColumn (_("Автор"), 6, wxALIGN_LEFT);
		m_BookList->AddColumn (_("№"), 2, wxALIGN_RIGHT);
		m_BookList->AddColumn (_("Жанр"), 4, wxALIGN_LEFT);
		m_BookList->AddColumn (_("Рейтинг"), 3, wxALIGN_LEFT);
		m_BookList->AddColumn (_("Язык"), 2, wxALIGN_LEFT);
		m_BookList->AddColumn (_("Тип"), 2, wxALIGN_LEFT);
		m_BookList->AddColumn (_("Размер, Кб"), 3, wxALIGN_RIGHT);
	}
	CreateColumns( (bool) FbParams::GetValue(keyMode) ? FB2_MODE_TREE : FB2_MODE_LIST );
}

int FbBookPanel::GetRatingColumn()
{
	return 4;
}

int FbBookPanel::GetOrderID()
{
	int col = abs( m_BookList->GetSortedColumn() );
	switch (col) {
		case 1: return ID_ORDER_TITLE;
		case 2: return ID_ORDER_AUTHOR;
		case 4: return ID_ORDER_GENRE;
		case 5: return ID_ORDER_RATING;
		case 6: return ID_ORDER_LANG;
		case 7: return ID_ORDER_TYPE;
		case 8: return ID_ORDER_SIZE;
		default: return ID_ORDER_AUTHOR;
	}
}

void FbBookPanel::SetOrderID(int id)
{
	int col = 0;
	switch (id) {
		case ID_ORDER_TITLE:  col = 1; break;
		case ID_ORDER_AUTHOR: col = 2; break;
		case ID_ORDER_GENRE:  col = 4; break;
		case ID_ORDER_RATING: col = 5; break;
		case ID_ORDER_LANG:   col = 6; break;
		case ID_ORDER_TYPE:   col = 7; break;
		case ID_ORDER_SIZE:   col = 8; break;
		default: col = 0;
	}
	if (IsOrderDesc()) col *= -1;
	m_BookList->SetSortedColumn(col);
	m_BookList->Update();
}

wxString FbBookPanel::GetOrderSQL()
{
	wxString order;
	int col = m_BookList->GetSortedColumn();
	switch ( abs(col) ) {
		case  1: order = wxT("title,full_name"); break;
		case  2: order = wxT("full_name,title"); break;
		case  3: order = wxT("full_name,title"); break;
		case  4: order = wxT("genres,full_name,title"); break;
		case  5: order = wxT("rating,full_name,title"); break;
		case  6: order = wxT("lang,full_name,title"); break;
		case  7: order = wxT("file_type,full_name,title"); break;
		case  8: order = wxT("file_size,full_name,title"); break;
		case  9: order = wxT("created,full_name,title"); break;
		default: order = wxT("title,full_name");
	}
	if ( col >= 0 ) return order;

	int pos;
	wxString result;
	do {
		pos = order.Find(wxT(","));
		if ( !result.IsEmpty() ) result += wxT(",");
		result += pos == wxNOT_FOUND ? order : order.Left(pos);
		result += wxT(" desc");
		order = order.Mid(pos + 1);
	} while (pos != wxNOT_FOUND);
	return result;
}

bool FbBookPanel::IsOrderDesc()
{
	return m_BookList->GetSortedColumn() < 0;
}

void FbBookPanel::RevertOrder()
{
	int col = - m_BookList->GetSortedColumn();
	m_BookList->SetSortedColumn(col);
}

void FbBookPanel::SetViewMode(int mode)
{
	Unsplit(m_BookInfo);
	m_BookInfo->SetPage(wxEmptyString);

	switch (mode) {
		case FB2_VIEW_NOTHING: return;
		case FB2_VIEW_VERTICAL: SplitVertically(m_BookList, m_BookInfo, GetSize().GetWidth()/2); break;
		default: SplitHorizontally(m_BookList, m_BookInfo, GetSize().GetHeight()/2);

	}

	FbItemData * book = GetSelectedBook();
	if (book) book->Show(this, mode == FB2_VIEW_VERTICAL);
}

FbItemData * FbBookPanel::GetSelectedBook()
{
	return (FbItemData*) m_BookList->GetSelectedData();
}

void FbBookPanel::OnBooksListViewSelected(wxTreeEvent & event)
{
	if (!IsSplit()) return;
	m_BookInfo->SetPage(wxEmptyString);
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbItemData * data = (FbItemData*) m_BookList->GetItemData(selected);
		if (data) data->Show(this, GetSplitMode() == wxSPLIT_VERTICAL);
	}
}

void FbBookPanel::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbItemData * data = (FbItemData*)m_BookList->GetItemData(selected);
		if (data) data->Open();
	}
}

void FbBookPanel::OnInfoUpdate(wxCommandEvent& event)
{
	if (!IsSplit()) return;
	FbItemData * data = GetSelectedBook();
	if (data && (data->GetId() == event.GetInt())) {
		m_BookInfo->SetPage(event.GetString());
	}
}

void FbBookPanel::OnSubmenu(wxCommandEvent& event)
{
	wxPostEvent(m_BookList, event);
}

void FbBookPanel::OnContextMenu(wxTreeEvent& event)
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

void FbBookPanel::ShowContextMenu(const wxPoint& pos, wxTreeItemId item)
{
	if (!m_master) return;
	int id = 0;
	if (item.IsOk()) {
		FbItemData * data = (FbItemData*)m_BookList->GetItemData(item);
		if (data) id = data->GetId();
	}
	FbBookMenu menu(id, *m_master, GetListMode()==FB2_MODE_LIST);
	FbMenuFolders::Connect(this, wxCommandEventHandler(FbBookPanel::OnFolderAdd));
	FbMenuAuthors::Connect(this, wxCommandEventHandler(FbBookPanel::OnOpenAuthor));
	PopupMenu(&menu, pos.x, pos.y);
}

void FbBookPanel::OnSelectAll(wxCommandEvent& event)
{
	m_BookList->SelectAll(1);
}

void FbBookPanel::OnUnselectAll(wxCommandEvent& event)
{
	m_BookList->SelectAll(0);
}

void FbBookPanel::OnOpenBook(wxCommandEvent & event)
{
	FbItemData * data = GetSelectedBook();
	if (data) data->Open();
}

void FbBookPanel::OnFavoritesAdd(wxCommandEvent & event)
{
	DoFolderAdd( 0 );
}

void FbBookPanel::OnFolderAdd(wxCommandEvent& event)
{
	DoFolderAdd( FbMenuFolders::GetFolder(event.GetId()) );
}

void FbBookPanel::DoFolderAdd(const int folder)
{
	wxString sel = m_BookList->GetSelected();
	wxString sql = wxString::Format(wxT("\
		INSERT INTO favorites(id_folder,md5sum) \
		SELECT DISTINCT %d, md5sum FROM books WHERE id IN (%s) \
	"), folder, sel.c_str());

	wxThread * thread = new FbFolderUpdateThread( sql, folder, FT_FOLDER );
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbBookPanel::OnChangeRating(wxCommandEvent& event)
{
	int iRating = event.GetId() - ID_RATING_0;

	wxString sRating;
	if (iRating) sRating = wxT(" ") + strRating[iRating];
	size_t count = m_BookList->UpdateRating(GetRatingColumn(), sRating);
	if ( !count ) return;

	m_BookList->Update();

	wxString sel = m_BookList->GetSelected();

	wxString sql1 = wxString::Format(wxT("\
		UPDATE states SET rating=%d WHERE md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), iRating, sel.c_str());

	wxString sql2 = wxString::Format(wxT("\
		INSERT INTO states(md5sum, rating) \
		SELECT DISTINCT md5sum, %d FROM books WHERE id IN (%s) \
		AND NOT EXISTS (SELECT rating FROM states WHERE states.md5sum = books.md5sum) \
	"), iRating, sel.c_str());

	wxThread * thread = new FbFolderUpdateThread( sql1, iRating, FT_RATING, sql2 );
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbBookPanel::DoDeleteDownload(const wxString &sel, const int folder)
{
	wxString sql1 = wxString::Format(wxT("\
		UPDATE states SET download=%d WHERE md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id>0 AND id IN (%s)) \
	"), folder, sel.c_str());

	wxThread * thread = new FbFolderUpdateThread( sql1, folder, FT_DOWNLOAD );
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbBookPanel::DoCreateDownload(const wxString &sel, int count)
{
	int folder = FbLocalDatabase().NewId(FB_NEW_DOWNLOAD, count) - count + 1;

	wxString sql1 = wxString::Format(wxT("\
		INSERT INTO states(md5sum, download) \
		SELECT DISTINCT md5sum, 1 FROM books WHERE id>0 AND id IN (%s) \
		AND NOT EXISTS (SELECT rating FROM states WHERE states.md5sum = books.md5sum) \
	"), sel.c_str());

	wxString sql2 = wxString::Format(wxT("\
		UPDATE states SET download=INCREMENT(%d) WHERE md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id>0 AND id IN (%s)) \
	"), folder, sel.c_str());

	wxThread * thread = new FbCreateDownloadThread( sql1, folder, FT_DOWNLOAD, sql2 );
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbBookPanel::OnDownloadBook(wxCommandEvent & event)
{
	wxString sel;
	size_t count = m_BookList->GetSelected(sel);
	if (count) DoCreateDownload(sel, count);
}

void FbBookPanel::OnDeleteDownload(wxCommandEvent & event)
{
	wxString sel = m_BookList->GetSelected();
	DoDeleteDownload(sel, 0);
}

void FbBookPanel::OnEditComments(wxCommandEvent & event)
{
	wxTreeItemId selected = m_BookList->GetSelection();
	if (selected.IsOk()) {
		FbItemData * data = (FbItemData*)m_BookList->GetItemData(selected);
		if (data) new FbFrameHtml((wxAuiMDIParentFrame*)wxGetApp().GetTopWindow(), data->GetId());
	}
}

void FbBookPanel::OnOpenAuthor(wxCommandEvent& event)
{
	int author = FbMenuAuthors::GetAuthor(event.GetId());
	if (author == 0) return;
	FbItemData * data = GetSelectedBook();
	if (data) FbOpenEvent(ID_BOOK_AUTHOR, author, data->GetId()).Post();
}

void FbBookPanel::EmptyBooks(const int selected)
{
	m_AuthorItem = 0L;
	m_SequenceItem = 0L;
	if (selected) {
		m_selected = selected;
	} else {
		FbItemData * data = GetSelectedBook();
		if (data) m_selected = data->GetId();
	}

	BookListUpdater updater(m_BookList);
	wxTreeItemId root = m_BookList->AddRoot(wxEmptyString);
	m_BookInfo->SetPage(wxEmptyString);
}

void FbBookPanel::AppendAuthor(int id, const wxString title, wxTreeItemData * data)
{
	FbTreeListUpdater updater(m_BookList);
	wxTreeItemId parent = m_BookList->GetRootItem();
	m_AuthorItem = m_BookList->AppendItem(parent, title, 0, -1, data);
	m_BookList->SetItemBold(m_AuthorItem, true);
	m_BookList->Expand(parent);
}

void FbBookPanel::AppendSequence(int id, const wxString title, wxTreeItemData * data)
{
	if (title.IsEmpty() && !m_SequenceItem.IsOk()) {
		m_SequenceItem = m_AuthorItem;
		return;
	}
	FbTreeListUpdater updater(m_BookList);
	wxTreeItemId parent = m_AuthorItem.IsOk() ? m_AuthorItem : m_BookList->GetRootItem();
	m_SequenceItem = m_BookList->AppendItem(parent, title, 0, -1, data);
	m_BookList->SetItemBold(m_SequenceItem, true);
	m_BookList->Expand(parent);
}

wxString FbBookPanel::F(const int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return F(hi) + wxT(" ") + wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

void FbBookPanel::AppendBook(BookTreeItemData & data, const wxString & authors)
{
	FbTreeListUpdater updater(m_BookList);

	wxString file_size = F(data.file_size/1024) + wxT(" ");
	wxTreeItemId parent;
	wxString sRating;
	if (data.rating) sRating = wxT(" ") + strRating[data.rating];

	wxTreeItemId item;
	switch (m_ListMode) {
		case FB2_MODE_TREE: {
			parent = m_SequenceItem.IsOk() ? m_SequenceItem : ( m_AuthorItem.IsOk() ? m_AuthorItem : m_BookList->GetRootItem() );
			item = m_BookList->AppendItem(parent, data.title, 0, -1, new FbBookData(data));
			if (data.number) m_BookList->SetItemText(item, 2, wxString::Format(wxT(" %d "), data.number));
		} break;
		case FB2_MODE_LIST: {
			parent = m_BookList->GetRootItem();
			item = m_BookList->AppendItem(parent, data.title, 0, -1, new FbBookData(data));
			m_BookList->SetItemText(item, 1, authors);
		} break;
	}
	m_BookList->SetItemText(item, 3, data.genres);
	m_BookList->SetItemText(item, 4, sRating);
	m_BookList->SetItemText(item, 5, data.language);
	m_BookList->SetItemText(item, 6, data.file_type);
	m_BookList->SetItemText(item, 7, file_size);

	m_BookList->Expand(parent);
	if (data.GetId() == m_selected) m_BookList->SelectItem(item);
}

void FbBookPanel::CreateColumns(FbListMode mode)
{
	m_ListMode = mode;

	m_AuthorItem = 0L;
	m_SequenceItem = 0L;

	BookListUpdater updater(m_BookList);

	switch (m_ListMode) {
		case FB2_MODE_TREE: {
			m_BookList->SetColumnShown(1, false);
			m_BookList->SetColumnShown(2, true);
		} break;
		case FB2_MODE_LIST: {
			m_BookList->SetColumnShown(1, true);
			m_BookList->SetColumnShown(2, false);
		} break;
	}

	wxTreeItemId root = m_BookList->AddRoot(wxEmptyString);
	m_BookInfo->SetPage(wxEmptyString);

	m_BookList->SetSortedColumn( m_ListMode == FB2_MODE_LIST ? 1 : 0 );
}

void FbBookPanel::OnSystemDownload(wxCommandEvent & event)
{
	FbItemData * data = GetSelectedBook();
	if (data && data->GetId()>0) {
		wxString url = FbInternetBook::GetURL(data->GetId());
		wxLaunchDefaultBrowser(url);
	}
}

void FbBookPanel::OnBookPage(wxCommandEvent & event)
{
	FbItemData * data = GetSelectedBook();
	if (data && data->GetId()>0) {
		wxString host = FbParams::GetText(DB_DOWNLOAD_HOST);
		wxString url = wxString::Format(wxT("http://%s/b/%d"), host.c_str(), data->GetId());
		wxLaunchDefaultBrowser(url);
	}
}

void FbBookPanel::UpdateFonts(bool refresh)
{
	m_BookList->SetFont( FbParams::GetFont(FB_FONT_MAIN) );
	m_BookList->SetColumnShown(3, FbParams::GetValue(FB_COLUMN_GENRE));
	m_BookList->SetColumnShown(4, FbParams::GetValue(FB_COLUMN_RATING));
	m_BookList->SetColumnShown(5, FbParams::GetValue(FB_COLUMN_LANG));
	m_BookList->SetColumnShown(6, FbParams::GetValue(FB_COLUMN_TYPE));
	m_BookList->SetColumnShown(7, FbParams::GetValue(FB_COLUMN_SYZE));
	if (refresh) m_BookList->Update();

	if (refresh) m_BookInfo->SetPage(wxEmptyString);
	FbAuiMDIChildFrame::UpdateFont(m_BookInfo, refresh);
	if (refresh && IsSplit()) {
		FbItemData * data = GetSelectedBook();
		if (data) data->Show(this, GetSplitMode() == wxSPLIT_VERTICAL);
	}
}

void FbBookPanel::UpdateInfo(int id)
{
	FbItemData * data = GetSelectedBook();
	if (data && data->GetId()==id) {
		InfoCash::UpdateInfo(this, id, GetSplitMode() == wxSPLIT_VERTICAL);
	}
}

void FbBookPanel::OnDeleteBooks(wxCommandEvent& event)
{
	wxString sel;
	wxArrayInt items;
	m_BookList->GetSelected(items);
	size_t count = m_BookList->GetSelected(sel);
	if (!count) return;

	wxString msg = wxString::Format(_("Удалить выделенные книги (%d шт.)?"), count);
	int answer = wxMessageBox(msg, _("Подтверждение"), wxOK | wxCANCEL, this);
	if (answer != wxOK) return;

	(new FbDeleteThread(sel))->Execute();
	m_BookList->DeleteItems(items);
}

void FbBookPanel::OnModifyBooks(wxCommandEvent& event)
{
	FbEditBookDlg::Execute();
}

void FbBookPanel::OnAuthorInfo(wxCommandEvent& event)
{
	if (IsSplit())  try {
		m_BookInfo->SetPage( event.GetString() );
		m_selected = 0;
	} catch (...) { }
}

void FbBookPanel::OnChangeView(wxCommandEvent & event)
{
	int mode = FB2_VIEW_HORISONTAL;
	switch (event.GetId()) {
		case ID_SPLIT_VERTICAL: mode = FB2_VIEW_VERTICAL; break;
		case ID_SPLIT_NOTHING: mode = FB2_VIEW_NOTHING; break;
	}
	SetViewMode(mode);
	if (m_KeyView) FbParams().SetValue(m_KeyView, mode);
}

FbViewMode FbBookPanel::GetViewMode()
{
	if (IsSplit())
		switch (GetSplitMode()) {
			case wxSPLIT_HORIZONTAL: return FB2_VIEW_HORISONTAL;
			case wxSPLIT_VERTICAL: return FB2_VIEW_VERTICAL;
		}
	return FB2_VIEW_NOTHING;
}

void FbBookPanel::SetMasterData(FbMasterData const * master)
{
	wxDELETE(m_master);
	if (master) m_master = master->Clone(); else return;

	switch ( master->GetType() ) {
		case FT_AUTHOR: {
			m_BookList->SetColumnShown(1, false);
		} break;
		case FT_GENRE: {
			m_BookList->SetColumnShown(3, false);
		} break;
		default: {
			m_BookList->SetColumnShown(2, GetListMode() == FB2_MODE_LIST);
			m_BookList->SetColumnShown(3, FbParams::GetValue(FB_COLUMN_GENRE));
		}
	}
};
