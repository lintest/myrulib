#include "FbBookPanel.h"
#include "FbConst.h"
#include "FbManager.h"
#include "FbFrameHtml.h"
#include "InfoCash.h"
#include "FbBookMenu.h"
#include "MyRuLibApp.h"
#include "FbDownloader.h"
#include "FbUpdateThread.h"
#include "FbEditBook.h"
#include "ZipReader.h"

class FbURI: public wxURI
{
	public:
		FbURI(const wxString& uri): wxURI(uri) {};
		friend class FbHtmlWindow;
};

class FbHtmlWindow: public wxHtmlWindow
{
	public:
		FbHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY);
	protected:
		virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const;
};

FbHtmlWindow::FbHtmlWindow(wxWindow *parent, wxWindowID id)
	: wxHtmlWindow(parent, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
}

wxHtmlOpeningStatus FbHtmlWindow::OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const
{
	if (type != wxHTML_URL_IMAGE) return wxHTML_OPEN;

	wxString addr = url;
	FbURI uri = addr;

	if (uri.GetScheme() == wxT("http")) {
		if ( !FbParams::GetValue(FB_HTTP_IMAGES) ) {
			*redirect = wxT("memory:blank");
			return wxHTML_REDIRECT;
		}
	}

	return wxHTML_OPEN;
}

BEGIN_EVENT_TABLE(FbBookPanel, wxSplitterWindow)
	EVT_MENU(ID_BOOKINFO_UPDATE, FbBookPanel::OnInfoUpdate)
	EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, FbBookPanel::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, FbBookPanel::OnBooksListActivated)
	EVT_TREE_ITEM_MENU(ID_BOOKS_LISTCTRL, FbBookPanel::OnContextMenu)
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
	EVT_HTML_LINK_CLICKED(ID_BOOKS_INFO_PANEL, FbBookPanel::OnLinkClicked)
END_EVENT_TABLE()

FbBookPanel::FbBookPanel()
	:wxSplitterWindow(), m_BookInfo(NULL), m_folder(fbNO_FOLDER), m_type(0), m_selected(0)
{
}

FbBookPanel::FbBookPanel(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode)
	:wxSplitterWindow(), m_BookInfo(NULL), m_folder(fbNO_FOLDER), m_type(0), m_selected(0)
{
	Create(parent, size, style, keyType, keyMode);
}

bool FbBookPanel::Create(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode)
{
	bool res = wxSplitterWindow::Create(parent, wxID_ANY, wxDefaultPosition, size, wxSP_NOBORDER, wxT("bookspanel"));
	if (res) {
		SetMinimumPaneSize(50);
		SetSashGravity(0.5);
		m_BookList = new FbBookList(this, ID_BOOKS_LISTCTRL, style);
		CreateBookInfo( (bool) FbParams::GetValue(keyType) );
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
	return res;
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
	int col = m_BookList->GetSortedColumn();
	switch (col) {
		case -9: return wxT("created desc,full_name desc,title desc");
		case -8: return wxT("file_size desc,full_name desc,title desc");
		case -7: return wxT("file_type desc,full_name desc,title desc");
		case -6: return wxT("lang desc,full_name desc,title desc");
		case -5: return wxT("rating desc,full_name desc,title desc");
		case -4: return wxT("genres desc,full_name desc,title desc");
		case -3: return wxT("full_name desc,title desc");
		case -2: return wxT("full_name desc,title desc");
		case -1: return wxT("title desc,full_name desc");
		case  1: return wxT("title,full_name");
		case  2: return wxT("full_name,title");
		case  3: return wxT("full_name,title");
		case  4: return wxT("genres,full_name,title");
		case  5: return wxT("rating,full_name,title");
		case  6: return wxT("lang,full_name,title");
		case  7: return wxT("file_type,full_name,title");
		case  8: return wxT("file_size,full_name,title");
		case  9: return wxT("created,full_name,title");
		default: return wxT("title,full_name");
	}
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

void FbBookPanel::CreateBookInfo(bool bVertical)
{
	if (m_BookInfo) Unsplit(m_BookInfo);

	m_BookInfo = new FbHtmlWindow(this, ID_BOOKS_INFO_PANEL);

	if (bVertical)
		SplitVertically(m_BookList, m_BookInfo, GetSize().GetWidth()/2);
	else
		SplitHorizontally(m_BookList, m_BookInfo, GetSize().GetHeight()/2);

	FbBookData * book = GetSelectedBook();
	if (!book) {
		m_BookInfo->SetPage(wxEmptyString);
	} else {
		InfoCash::LoadIcon(book->m_filetype);
		InfoCash::UpdateInfo(this, book->GetId(), bVertical);
	}
}

FbBookData * FbBookPanel::GetSelectedBook()
{
	return (FbBookData*) m_BookList->GetSelectedData();
}

void FbBookPanel::OnBooksListViewSelected(wxTreeEvent & event)
{
	m_BookInfo->SetPage(wxEmptyString);

	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbBookData * data = (FbBookData*) m_BookList->GetItemData(selected);
		if (data) {
			if (data->GetId()) {
				InfoCash::LoadIcon(data->m_filetype);
				InfoCash::UpdateInfo(this, data->GetId(), GetSplitMode() == wxSPLIT_VERTICAL);
			} else if (data->GetAuthor()) {
				(new AuthorThread(this->GetParent(), data->GetAuthor()))->Execute();
			}
		}
	}
}

void FbBookPanel::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		FbBookData * data = (FbBookData*)m_BookList->GetItemData(selected);
		if (data) DoOpenBook(data->GetId(), data->m_filetype);
	}
}

void FbBookPanel::OnInfoUpdate(wxCommandEvent& event)
{
	FbBookData * data= GetSelectedBook();
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
	int id = 0;
	if (item.IsOk()) {
		FbBookData * data = (FbBookData*)m_BookList->GetItemData(item);
		if (data) id = data->GetId();
	}
	FbBookMenu menu(id, m_folder, m_type, GetListMode()==FB2_MODE_LIST);
	FbMenuFolders::Connect(this, wxCommandEventHandler(FbBookPanel::OnFolderAdd));
	FbMenuAuthors::Connect(this, wxCommandEventHandler(FbBookPanel::OnOpenAuthor));
	PopupMenu(&menu, pos.x, pos.y);
}

void FbBookPanel::OnSelectAll(wxCommandEvent& event)
{
	m_BookList->SelectAll();
}

void FbBookPanel::OnUnselectAll(wxCommandEvent& event)
{
	m_BookList->SelectAll(0);
}

void FbBookPanel::OnOpenBook(wxCommandEvent & event)
{
	FbBookData * data = GetSelectedBook();
	if (data) DoOpenBook(data->GetId(), data->m_filetype);
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

int FbBookPanel::UpdateChildRating(wxTreeItemId parent, int iRating, const wxString &sRating)
{
	int result = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_BookList->GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		if (m_BookList->GetItemImage(child) == 1) {
			FbBookData * data = (FbBookData*) m_BookList->GetItemData(child);
			if (data && data->GetId()) {
				m_BookList->SetItemText(child, GetRatingColumn(), sRating);
				result++;
			}
		}
		result += UpdateChildRating(child, iRating, sRating);
		child = m_BookList->GetNextChild(parent, cookie);
	}
	return result;
}

int FbBookPanel::UpdateSelectionRating(int iRating, const wxString &sRating)
{
	wxArrayTreeItemIds items;
	size_t count = m_BookList->GetSelections(items);
	for (size_t i=0; i<count; ++i) {
		FbBookData * data = (FbBookData*) m_BookList->GetItemData(items[i]);
		if (data && data->GetId()) {
			m_BookList->SetItemText(items[i], GetRatingColumn(), sRating);
		}
	}
	return count;
}

void FbBookPanel::OnChangeRating(wxCommandEvent& event)
{
	int iRating = event.GetId() - ID_RATING_0;

	wxString sRating;
	if (iRating) sRating = wxT(" ") + strRating[iRating];
	int iUpdated = UpdateChildRating( m_BookList->GetRootItem(), iRating, sRating);
	if ( !iUpdated ) UpdateSelectionRating(iRating, sRating);

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

void FbBookPanel::DoOpenDownload(int id)
{
	if (id<0) return;

	wxString md5sum;

	try {
		wxString sql = wxT("SELECT md5sum FROM books WHERE id=?");
		FbCommonDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) md5sum = result.GetAsString(0);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return;
	}

	if (md5sum.IsEmpty()) return;

	try {
		bool ok = false;
		FbLocalDatabase database;
		int folder = database.NewId(FB_NEW_DOWNLOAD);
		{
			wxString sql = wxT("UPDATE states SET download=? WHERE md5sum=?");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, folder);
			stmt.Bind(2, md5sum);
			ok = stmt.ExecuteUpdate();
		}
		if (!ok) {
			wxString sql = wxT("INSERT INTO states(download, md5sum) VALUES (?,?)");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, folder);
			stmt.Bind(2, md5sum);
			stmt.ExecuteUpdate();
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return;
	}

	FbDownloader::Push(md5sum);
	FbDownloader::Start();
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
		FbBookData * data = (FbBookData*)m_BookList->GetItemData(selected);
		if (data) new FbFrameHtml((wxAuiMDIParentFrame*)wxGetApp().GetTopWindow(), data->GetId());
	}
}

void FbBookPanel::OnOpenAuthor(wxCommandEvent& event)
{
	int author = FbMenuAuthors::GetAuthor(event.GetId());
	if (author == 0) return;

	FbBookData * data = GetSelectedBook();
	if (data) FbOpenEvent(ID_BOOK_AUTHOR, author, data->GetId()).Post();
}

void FbBookPanel::ShowHTML(const wxString &html)
{
	m_BookInfo->SetPage( html );
	m_selected = 0;
}

void FbBookPanel::EmptyBooks(const int selected)
{
	m_AuthorItem = 0L;
	m_SequenceItem = 0L;
	if (selected) {
		m_selected = selected;
	} else {
		FbBookData * data = GetSelectedBook();
		if (data) m_selected = data->GetId();
	}

	BookListUpdater updater(m_BookList);
	wxTreeItemId root = m_BookList->AddRoot(wxEmptyString);
	m_BookInfo->SetPage(wxEmptyString);
}

void FbBookPanel::AppendAuthor(int id, const wxString title)
{
	FbTreeListUpdater updater(m_BookList);
	wxTreeItemId parent = m_BookList->GetRootItem();
	m_AuthorItem = m_BookList->AppendItem(parent, title, 0, -1, new FbBookData(0, id));
	m_BookList->SetItemBold(m_AuthorItem, true);
	m_BookList->Expand(parent);
}

void FbBookPanel::AppendSequence(const wxString title)
{
	if (title.IsEmpty() && !m_SequenceItem.IsOk()) {
		m_SequenceItem = m_AuthorItem;
		return;
	}
	wxString text = title.IsEmpty() ? strOtherSequence : title;
	FbTreeListUpdater updater(m_BookList);
	wxTreeItemId parent = m_AuthorItem.IsOk() ? m_AuthorItem : m_BookList->GetRootItem();
	m_SequenceItem = m_BookList->AppendItem(parent, text, 0);
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
	FbBookData * data = GetSelectedBook();
	if (data && data->GetId()>0) {
		wxString url = FbInternetBook::GetURL(data->GetId());
		wxLaunchDefaultBrowser(url);
	}
}

void FbBookPanel::OnBookPage(wxCommandEvent & event)
{
	FbBookData * data = GetSelectedBook();
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
	if (refresh) {
		FbBookData * data = GetSelectedBook();
		if (data && data->GetId()) {
			InfoCash::UpdateInfo(this, data->GetId(), GetSplitMode() == wxSPLIT_VERTICAL);
		}
	}
}

void FbBookPanel::UpdateInfo(int id)
{
	FbBookData * data = GetSelectedBook();
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

void FbBookPanel::OnLinkClicked(wxHtmlLinkEvent& event)
{
	wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}

void * FbBookPanel::AuthorThread::Entry()
{
	FbCommonDatabase database;
	wxString sql = wxT("SELECT description FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_author);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) FbCommandEvent(fbEVT_BOOK_ACTION, ID_AUTHOR_INFO, m_author, result.GetString(0)).Post(m_frame);
	return NULL;
}

void FbBookPanel::DoOpenBook(int id, const wxString &file_type)
{
	ZipReader reader(id, id<0);
	if ( reader.IsOK() ) {
		FbManager::OpenBook( reader.GetZip(), file_type);
	} else if ( id>0 ) {
		if ( wxMessageBox(_("Скачать книгу?"), _("Подтверждение"), wxOK | wxCANCEL) == wxOK) DoOpenDownload(id);
	}
}

