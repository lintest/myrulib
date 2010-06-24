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
#include "FbMasterThread.h"
#include "FbBookList.h"
#include "FbBookTree.h"

IMPLEMENT_CLASS(FbBookPanel, wxSplitterWindow)

BEGIN_EVENT_TABLE(FbBookPanel, wxSplitterWindow)
	EVT_COMMAND(ID_AUTHOR_INFO, fbEVT_BOOK_ACTION, FbBookPanel::OnAuthorInfo)
	EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, FbBookPanel::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, FbBookPanel::OnBooksListActivated)
	EVT_TREE_ITEM_MENU(ID_BOOKS_LISTCTRL, FbBookPanel::OnContextMenu)
	EVT_HTML_LINK_CLICKED(ID_BOOKS_INFO_PANEL, FbBookPanel::OnLinkClicked)
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
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbBookPanel::OnListModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbBookPanel::OnListArray)
	EVT_FB_MODEL(ID_MODEL_CREATE, FbBookPanel::OnTreeModel)
END_EVENT_TABLE()

FbBookPanel::FbBookPanel(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode)
	: wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, size, wxSP_NOBORDER, wxT("bookspanel")),
		m_BookInfo(NULL), m_selected(0), m_KeyView(keyType), m_thread(new FbMasterThread(this))
{
	SetMinimumPaneSize(50);
	SetSashGravity(0.5);

	long substyle = wxBORDER_SUNKEN | fbTR_VRULES | fbTR_MULTIPLE | fbTR_CHECKBOX;
	m_BookList = new FbTreeViewCtrl(this, ID_BOOKS_LISTCTRL, wxDefaultPosition, wxDefaultSize, substyle);
	m_BookInfo = new FbPreviewWindow(this, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

	int mode = FbParams::GetValue(keyType);
	if (mode == FB2_VIEW_NOTHING) {
		Initialize(m_BookList);
		m_BookInfo->Show(false);
	} else {
		SetViewMode(mode);
	}

	FbListMode listmode = (bool) FbParams::GetValue(keyMode) ? FB2_MODE_TREE : FB2_MODE_LIST;
	CreateColumns(listmode);
	m_thread->Execute();
}

FbBookPanel::~FbBookPanel()
{
	m_thread->Close();
	m_thread->Delete();
}

void FbBookPanel::Localize()
{
	m_BookList->EmptyColumns();
	CreateColumns(GetListMode());
}

void FbBookPanel::CreateColumns(FbListMode mode)
{
	m_ListMode = mode;
	m_BookList->EmptyColumns();
	m_BookList->AddColumn(BF_NAME, _("Title"), 10, wxALIGN_LEFT);
	m_BookList->AddColumn(BF_AUTH, _("Author"), 6, wxALIGN_LEFT);
	m_BookList->AddColumn(BF_NUMB, _("#"), 2, wxALIGN_RIGHT);
	m_BookList->AddColumn(BF_GENR, _("Genre"), 4, wxALIGN_LEFT);
	m_BookList->AddColumn(BF_RATE, _("Rating"), 3, wxALIGN_LEFT);
	m_BookList->AddColumn(BF_LANG, _("Language"), 2, wxALIGN_LEFT);
	m_BookList->AddColumn(BF_TYPE, _("Extension"), 2, wxALIGN_LEFT);
	m_BookList->AddColumn(BF_SIZE, _("Size, Kb"), 3, wxALIGN_RIGHT);
	m_BookList->SetSortedColumn( mode == FB2_MODE_TREE ? 0 : 1);
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
	ResetPreview();
}

void FbBookPanel::ResetPreview()
{
	m_BookInfo->SetPage(wxEmptyString);
	if (!IsSplit()) return;

	FbViewContext ctx;
	FbModelItem item = m_BookList->GetCurrent();
	if (item) 
		m_BookInfo->Reset(ctx, (&item)->GetView());
	else m_BookInfo->Reset(ctx, FbViewItem());
}

void FbBookPanel::OnBooksListViewSelected(wxTreeEvent & event)
{
	ResetPreview();
}

void FbBookPanel::OnBooksListActivated(wxTreeEvent & event)
{
	int id = m_BookList->GetBook();
	if (id) FbBookData(id).Open();
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
	FbBookMenu menu(m_master, m_BookList->GetBook(), GetListMode()==FB2_MODE_LIST);
	FbMenuFolders::Connect(this, wxCommandEventHandler(FbBookPanel::OnFolderAdd));
	FbMenuAuthors::Connect(this, wxCommandEventHandler(FbBookPanel::OnOpenAuthor));
	PopupMenu(&menu, pos.x, pos.y);
}

void FbBookPanel::OnSelectAll(wxCommandEvent& event)
{
	m_BookList->SelectAll(true);
}

void FbBookPanel::OnUnselectAll(wxCommandEvent& event)
{
	m_BookList->SelectAll(false);
}

void FbBookPanel::OnOpenBook(wxCommandEvent & event)
{
	int id = m_BookList->GetBook();
	if (id) FbBookData(id).Open();
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
	wxString sel = GetSelected();
	wxString sql = wxString::Format(wxT("\
		INSERT INTO favorites(id_folder,md5sum) \
		SELECT DISTINCT %d, md5sum FROM books WHERE id IN (%s) \
	"), folder, sel.c_str());

	wxThread * thread = new FbFolderUpdateThread( sql, folder, FT_FOLDER );
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbBookPanel::OnChangeRating(wxCommandEvent& event)
{
/*
	int iRating = event.GetId() - ID_RATING_0;

	wxString sRating;
	if (iRating) sRating = wxT(" ") + GetRatingText(iRating);
	size_t count = m_BookList->UpdateRating(GetRatingColumn(), sRating);
	if ( !count ) return;

	m_BookList->Update();

	wxString sel = GetSelected();

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
*/
}

void FbBookPanel::DoDeleteDownload(const wxString &sel, const int folder)
{
/*
	wxString sql1 = wxString::Format(wxT("\
		UPDATE states SET download=%d WHERE md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id>0 AND id IN (%s)) \
	"), folder, sel.c_str());

	wxThread * thread = new FbFolderUpdateThread( sql1, folder, FT_DOWNLOAD );
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
*/
}

void FbBookPanel::DoCreateDownload(const wxString &sel, int count)
{
/*
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
*/
}

void FbBookPanel::OnDownloadBook(wxCommandEvent & event)
{
	wxString sel;
	size_t count = GetSelected(sel);
	if (count) DoCreateDownload(sel, count);
}

void FbBookPanel::OnDeleteDownload(wxCommandEvent & event)
{
	wxString sel = GetSelected();
	DoDeleteDownload(sel, 0);
}

void FbBookPanel::OnEditComments(wxCommandEvent & event)
{
	int id = m_BookList->GetBook();
	if (id) new FbFrameHtml((wxAuiMDIParentFrame*)wxGetApp().GetTopWindow(), id);
}

void FbBookPanel::OnOpenAuthor(wxCommandEvent& event)
{
	int author = FbMenuAuthors::GetAuthor(event.GetId());
	int id = m_BookList->GetBook();
	if (id) FbOpenEvent(ID_BOOK_AUTHOR, author, id).Post();
}

void FbBookPanel::EmptyBooks(const int selected)
{
	m_BookList->AssignModel(NULL);
	m_BookInfo->SetPage(wxEmptyString);
}

void FbBookPanel::OnSystemDownload(wxCommandEvent & event)
{
	int id = m_BookList->GetBook();
	if (id > 0) {
		wxString url = FbInternetBook::GetURL(id);
		wxLaunchDefaultBrowser(url);
	}
}

void FbBookPanel::OnBookPage(wxCommandEvent & event)
{
	int id = m_BookList->GetBook();
	if (id > 0) {
		wxString host = FbParams::GetText(DB_DOWNLOAD_HOST);
		wxString url = wxString::Format(wxT("http://%s/b/%d"), host.c_str(), id);
		wxLaunchDefaultBrowser(url);
	}
}

void FbBookPanel::UpdateFonts(bool refresh)
{
	m_BookList->SetFont( FbParams::GetFont(FB_FONT_MAIN) );
/*
	m_BookList->SetColumnShown(3, FbParams::GetValue(FB_COLUMN_GENRE));
	m_BookList->SetColumnShown(4, FbParams::GetValue(FB_COLUMN_RATING));
	m_BookList->SetColumnShown(5, FbParams::GetValue(FB_COLUMN_LANG));
	m_BookList->SetColumnShown(6, FbParams::GetValue(FB_COLUMN_TYPE));
	m_BookList->SetColumnShown(7, FbParams::GetValue(FB_COLUMN_SYZE));
*/
	if (refresh) m_BookList->Update();
	if (refresh) m_BookInfo->SetPage(wxEmptyString);
	FbAuiMDIChildFrame::UpdateFont(m_BookInfo, refresh);
	if (refresh) ResetPreview();
}

void FbBookPanel::UpdateInfo(int id)
{
	if (m_BookList->GetBook() == id) {
		InfoCash::UpdateInfo(this, id, GetSplitMode() == wxSPLIT_VERTICAL);
	}
}

void FbBookPanel::OnDeleteBooks(wxCommandEvent& event)
{
/*
	wxString sel;
	wxArrayInt items;
	GetSelected(items);
	size_t count = GetSelected(sel);
	if (!count) return;

	wxString msg = wxString::Format(_("Delete selected books (%d pcs)?"), count);
	int answer = wxMessageBox(msg, _("Confirmation"), wxOK | wxCANCEL, this);
	if (answer != wxOK) return;

	(new FbDeleteThread(sel))->Execute();
	m_BookList->DeleteItems(items);
*/
}

void FbBookPanel::OnModifyBooks(wxCommandEvent& event)
{
/*
	FbEditBookDlg::Execute();
*/
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

void FbBookPanel::OnLinkClicked(wxHtmlLinkEvent& event)
{
	wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}

void FbBookPanel::OnListModel( FbArrayEvent& event )
{
	if (m_master.GetIndex() == event.GetInt()) {
		FbBookListModel * model = new FbBookListModel(event.GetArray());
		m_BookList->AssignModel(model);
	}
}

void FbBookPanel::OnListArray( FbArrayEvent& event )
{
	if (m_master.GetIndex() == event.GetInt()) {
		FbBookListModel * model = wxDynamicCast(m_BookList->GetModel(), FbBookListModel);
		if (model) model->Append(event.GetArray());
		m_BookList->Refresh();
	}
}

void FbBookPanel::OnTreeModel( FbModelEvent& event )
{
	if (m_master.GetIndex() == event.GetInt()) {
		m_BookList->AssignModel(event.GetModel());
	} else {
		delete event.GetModel();
	}
}

void FbBookPanel::Reset(const FbMasterInfo &master)
{
	m_master = master;

	m_BookInfo->SetPage(wxEmptyString);
	if (m_master) {
		m_BookList->AssignModel(NULL);
		m_master.SetOrder(m_BookList->GetSortedColumn());
		m_master.SetMode(GetListMode());
		m_thread->Reset(m_master);
	} else {
		m_BookList->AssignModel(new FbListStore);
	}
}

wxString FbBookPanel::GetSelected()
{
	wxString sel;
	GetSelected(sel);
	return sel;
}

size_t FbBookPanel::GetSelected(wxString &selections)
{
	selections.Empty();
	wxArrayInt items;
	GetSelected(items);
	size_t count = items.Count();
	for (size_t i = 0; i < count; i++) {
		if (i > 0) selections << wxT(',');
		selections << items[i];
	}
	return count;
}

size_t FbBookPanel::GetSelected(wxArrayInt &items)
{
	FbModel * model = m_BookList->GetModel();
	return model ? model->GetSelected(items) : 0;
}
