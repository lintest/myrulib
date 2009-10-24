#include "FbFrameFavour.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "FbFrameBaseThread.h"
#include "FbDownloader.h"
#include "res/start.xpm"
#include "res/pause.xpm"

BEGIN_EVENT_TABLE(FbFrameFavour, FbFrameBase)
	EVT_MENU(ID_FAVORITES_DEL, FbFrameFavour::OnFavoritesDel)
	EVT_MENU(ID_APPEND_FOLDER, FbFrameFavour::OnFolderAppend)
	EVT_MENU(ID_MODIFY_FOLDER, FbFrameFavour::OnFolderModify)
	EVT_MENU(ID_DELETE_FOLDER, FbFrameFavour::OnFolderDelete)
	EVT_MENU(ID_START, FbFrameFavour::OnStart)
	EVT_MENU(ID_PAUSE, FbFrameFavour::OnPause)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameFavour::OnFolderSelected)
END_EVENT_TABLE()

FbFrameFavour::FbFrameFavour(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_FAVOUR, _("Мои папки"))
{
	CreateControls();
}

void FbFrameFavour::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bToolSizer = new wxBoxSizer( wxHORIZONTAL );

	m_ToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS|wxTB_FLAT );
	m_ToolBar->AddTool( ID_APPEND_FOLDER, _("Добавить"), wxNullBitmap);
	m_ToolBar->AddTool( ID_MODIFY_FOLDER, _("Изменить"), wxNullBitmap);
	m_ToolBar->AddTool( ID_DELETE_FOLDER, _("Удалить"), wxNullBitmap);
	m_ToolBar->Realize();
	bToolSizer->Add( m_ToolBar, 0, wxALIGN_CENTER_VERTICAL);

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bToolSizer->Add( toolbar, 1, wxALIGN_CENTER_VERTICAL);

	bSizer1->Add( bToolSizer, 0, wxEXPAND);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxSUNKEN_BORDER | wxTR_NO_BUTTONS;
	m_FolderList = new FbTreeListCtrl(splitter, ID_MASTER_LIST, style);
	m_FolderList->AddColumn (_("Папки"), 100, wxALIGN_LEFT);
	m_FolderList->SetFocus();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_FolderList, &m_BooksPanel, 160);

	FillFolders();

	SetSizer( bSizer1 );
	Layout();
}

wxToolBar * FbFrameFavour::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->AddTool(ID_START, _("Старт"), wxBitmap(start_xpm), _("Начать загрузку файлов через интернет"));
	toolbar->AddTool(ID_PAUSE, _("Стоп"), wxBitmap(pause_xpm), _("Остановить загрузку файлов через интернет"));
	toolbar->Realize();
	return toolbar;
}

void FbFrameFavour::FillFolders(const int iCurrent)
{
	m_FolderList->Freeze();
	m_FolderList->DeleteRoot();

	wxTreeItemId root = m_FolderList->AddRoot(wxEmptyString);

	wxTreeItemId parent = m_FolderList->AppendItem(root, _("Закладки"));
	m_FolderList->SetItemBold(parent, true);

	wxTreeItemId item = m_FolderList->AppendItem(parent, _("Избранное"), -1, -1, new FbFolderData(0));
	if (iCurrent == 0) m_FolderList->SelectItem(item);

	wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		int id = result.GetInt(0);
		wxString name = result.GetString(1);
		wxTreeItemId item = m_FolderList->AppendItem(parent, name, -1, -1, new FbFolderData(id));
		if (iCurrent == id) m_FolderList->SelectItem(item);
	}
	m_FolderList->Expand(parent);
	m_FolderList->Expand(root);

	parent = m_FolderList->AppendItem(root, _("Пометки"));
	m_FolderList->SetItemBold(parent, true);

	m_FolderList->AppendItem(parent, _("Комментарии"), -1, -1, new FbFolderData(1, FT_COMMENT));
	m_FolderList->AppendItem(parent, strRating[5], -1, -1, new FbFolderData(5, FT_RATING));
	m_FolderList->AppendItem(parent, strRating[4], -1, -1, new FbFolderData(4, FT_RATING));
	m_FolderList->AppendItem(parent, strRating[3], -1, -1, new FbFolderData(3, FT_RATING));
	m_FolderList->AppendItem(parent, strRating[2], -1, -1, new FbFolderData(2, FT_RATING));
	m_FolderList->AppendItem(parent, strRating[1], -1, -1, new FbFolderData(1, FT_RATING));
	m_FolderList->Expand(parent);

	parent = m_FolderList->AppendItem(root, _("Закачки"));
	m_FolderList->SetItemBold(parent, true);

	m_FolderList->AppendItem(parent, wxT("Очередь"), -1, -1, new FbFolderData(1, FT_DOWNLOAD));
	m_FolderList->AppendItem(parent, wxT("Готово"), -1, -1, new FbFolderData(-1, FT_DOWNLOAD));
	m_FolderList->AppendItem(parent, wxT("Ошибки"), -1, -1, new FbFolderData(-2, FT_DOWNLOAD));
	m_FolderList->Expand(parent);

	m_FolderList->Thaw();
}

class FrameFavourThread: public FbFrameBaseThread
{
	public:
		FrameFavourThread(FbFrameBase * frame, FbListMode mode, FbFolderData * data)
			:FbFrameBaseThread(frame, mode), m_folder(data->GetId()), m_number(sm_skiper.NewNumber()), m_type(data->GetType()) {};
		virtual void *Entry();
	private:
		static FbThreadSkiper sm_skiper;
		int m_folder;
		int m_number;
		FbFolderType m_type;
};

FbThreadSkiper FrameFavourThread::sm_skiper;

void * FrameFavourThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition;

	switch (m_type) {
		case FT_FOLDER:
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM favorites WHERE id_folder=?)");
			break;
		case FT_RATING:
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE rating=?)");
			break;
		case FT_COMMENT:
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM comments WHERE ?>0)");
			break;
		case FT_DOWNLOAD: {
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE download");
			condition += ( m_folder==1 ? wxT(">=?)") : wxT("=?)") );
			} break;
	}
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		database.AttachConfig();
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_folder);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FbFrameFavour::OnFolderSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel.EmptyBooks();
		FbFolderData * data = (FbFolderData*) m_FolderList->GetItemData(selected);
		if (data) {
			bool enabled = data->GetType() == FT_FOLDER && data->GetId();
			m_ToolBar->EnableTool(ID_MODIFY_FOLDER, enabled);
			m_ToolBar->EnableTool(ID_DELETE_FOLDER, enabled);
			FillByFolder(data);
		}
	}
}

void FbFrameFavour::UpdateBooklist()
{
	FbFolderData * data = GetSelected();
	if (data) FillByFolder(data);
}

void FbFrameFavour::FillByFolder(FbFolderData * data)
{
	int iFolder = fbNO_FOLDER;
	switch (data->GetType()) {
		case FT_FOLDER:
			iFolder = data->GetId();
			break;
		case FT_DOWNLOAD:
			iFolder = fbFLDR_DOWN;
			break;
		default:
			iFolder = fbNO_FOLDER;
			break;
	}
	m_BooksPanel.SetFolder( iFolder );

	wxThread * thread = new FrameFavourThread(this, m_BooksPanel.GetListMode(), data);
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbFrameFavour::OnFavoritesDel(wxCommandEvent & event)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	int iFolder = data->GetId();

	wxString selected = m_BooksPanel.m_BookList->GetSelected();
	wxString sql = wxString::Format(wxT("DELETE FROM favorites WHERE md5sum IN (SELECT books.md5sum FROM books WHERE id IN (%s)) AND id_folder=%d"), selected.c_str(), iFolder);

	FbCommonDatabase database;
	database.AttachConfig();
	database.ExecuteUpdate(sql);

	FbTreeListUpdater updater(m_BooksPanel.m_BookList);

	wxArrayInt items;
	m_BooksPanel.m_BookList->GetSelected(items);
	m_BooksPanel.m_BookList->DeleteItems(items);
}

void FbFrameFavour::OnFolderAppend(wxCommandEvent & event)
{
	wxString name = wxGetTextFromUser(_("Введите имя новой папки:"), _("Добавить папку?"), wxEmptyString, this);
	if (name.IsEmpty()) return;

	FbLocalDatabase database;
	int id = database.NewId(FB_NEW_FOLDER);
	wxString sql = wxT("INSERT INTO folders(value,id) VALUES(?,?)");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, name);
	stmt.Bind(2, id);
	stmt.ExecuteUpdate();

	FbBookMenu::EmptyFolders();
	m_BooksPanel.EmptyBooks();
	FillFolders(id);
}

void FbFrameFavour::OnFolderModify(wxCommandEvent & event)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	if (data->GetType() != FT_FOLDER) return;
	int id = data->GetId();
	if (!id) return;

	wxTreeItemId item = m_FolderList->GetSelection();
	wxString name = m_FolderList->GetItemText(item);;
	name = wxGetTextFromUser(_("Введите новое имя папки:"), _("Изменить папку?"), name, this);
	if (name.IsEmpty()) return;

	FbLocalDatabase database;
	wxString sql = wxT("UPDATE folders SET value=? WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, name);
	stmt.Bind(2, id);
	stmt.ExecuteUpdate();

	FbBookMenu::EmptyFolders();
	FillFolders(id);
}

void FbFrameFavour::OnFolderDelete(wxCommandEvent & event)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	if (data->GetType() != FT_FOLDER) return;
	int id = data->GetId();
	if (!id) return;

	wxTreeItemId item = m_FolderList->GetSelection();
	wxString name = m_FolderList->GetItemText(item);;
	wxString msg = wxString::Format(_("Удалить папку «%s»?"), name.c_str());
	int answer = wxMessageBox(msg, _("Удалить папку?"), wxOK | wxCANCEL, this);
	if (answer != wxOK) return;

	FbLocalDatabase database;
	wxString sql = wxT("DELETE FROM folders WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	stmt.ExecuteUpdate();

	sql = wxT("DELETE FROM favorites WHERE id_folder=?");
	stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	stmt.ExecuteUpdate();

	FbBookMenu::EmptyFolders();
	m_BooksPanel.EmptyBooks();
	FillFolders(0);
}

FbFolderData * FbFrameFavour::GetSelected()
{
	wxTreeItemId item = m_FolderList->GetSelection();
	if (item.IsOk())
		return (FbFolderData * ) m_FolderList->GetItemData(item);
	else
		return NULL;
}

void FbFrameFavour::UpdateFolder(const int iFolder, const FbFolderType type)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	if (data->GetType()!= type) return;

	bool bNeedUpdate = false;
	switch (type) {
		case FT_FOLDER:
			bNeedUpdate = data->GetId()==iFolder;
			break;
		default:
			bNeedUpdate = true;
			break;
	}

	if (bNeedUpdate) FillByFolder(data);
}

void FbFrameFavour::OnStart(wxCommandEvent & event)
{
	FbDownloader::Start();
}

void FbFrameFavour::OnPause(wxCommandEvent & event)
{
	FbDownloader::Pause();
}
