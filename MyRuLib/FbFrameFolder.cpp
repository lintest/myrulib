#include "FbFrameFolder.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "FbDownloader.h"

BEGIN_EVENT_TABLE(FbFrameFolder, FbFrameBase)
	EVT_MENU(ID_FAVORITES_DEL, FbFrameFolder::OnFavoritesDel)
	EVT_MENU(ID_APPEND_FOLDER, FbFrameFolder::OnFolderAppend)
	EVT_MENU(ID_MODIFY_FOLDER, FbFrameFolder::OnFolderModify)
	EVT_MENU(ID_DELETE_FOLDER, FbFrameFolder::OnFolderDelete)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameFolder::OnFolderSelected)
END_EVENT_TABLE()

FbFrameFolder::FbFrameFolder(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_FOLDER, _("Мои папки"))
{
	CreateControls();
}

void FbFrameFolder::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bToolSizer = new wxBoxSizer( wxHORIZONTAL );

	m_ToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS|wxTB_FLAT );
	m_ToolBar->SetFont(FbParams::GetFont(FB_FONT_TOOL));
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
	m_MasterList = new FbTreeListCtrl(splitter, ID_MASTER_LIST, style);
	m_MasterList->AddColumn (_("Папки"), 100, wxALIGN_LEFT);
	m_MasterList->SetFocus();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FillFolders();

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

void FbFrameFolder::FillFolders(const int iCurrent)
{
	m_MasterList->Freeze();
	m_MasterList->DeleteRoot();

	wxTreeItemId root = m_MasterList->AddRoot(wxEmptyString);

	wxTreeItemId parent = m_MasterList->AppendItem(root, _("Закладки"));
	m_MasterList->SetItemBold(parent, true);

	wxTreeItemId item = m_MasterList->AppendItem(parent, _("Избранное"), -1, -1, new FbFolderData(0));
	if (iCurrent == 0) m_MasterList->SelectItem(item);

	wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		int id = result.GetInt(0);
		wxString name = result.GetString(1);
		wxTreeItemId item = m_MasterList->AppendItem(parent, name, -1, -1, new FbFolderData(id));
		if (iCurrent == id) m_MasterList->SelectItem(item);
	}
	m_MasterList->Expand(parent);
	m_MasterList->Expand(root);

	parent = m_MasterList->AppendItem(root, _("Пометки"));
	m_MasterList->SetItemBold(parent, true);

	m_MasterList->AppendItem(parent, _("Комментарии"), -1, -1, new FbFolderData(1, FT_COMMENT));
	m_MasterList->AppendItem(parent, strRating[5], -1, -1, new FbFolderData(5, FT_RATING));
	m_MasterList->AppendItem(parent, strRating[4], -1, -1, new FbFolderData(4, FT_RATING));
	m_MasterList->AppendItem(parent, strRating[3], -1, -1, new FbFolderData(3, FT_RATING));
	m_MasterList->AppendItem(parent, strRating[2], -1, -1, new FbFolderData(2, FT_RATING));
	m_MasterList->AppendItem(parent, strRating[1], -1, -1, new FbFolderData(1, FT_RATING));
	m_MasterList->Expand(parent);

	m_MasterList->Thaw();
}

FbThreadSkiper FbFrameFolder::FolderThread::sm_skiper;

void * FbFrameFolder::FolderThread::Entry()
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
		InitDatabase(database);
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

void FbFrameFolder::OnFolderSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbFolderData * data = (FbFolderData*) m_MasterList->GetItemData(selected);
		if (data) {
			bool enabled = data->GetType() == FT_FOLDER && data->GetId();
			m_ToolBar->EnableTool(ID_MODIFY_FOLDER, enabled);
			m_ToolBar->EnableTool(ID_DELETE_FOLDER, enabled);
			FillByFolder(data);
		}
	}
}

void FbFrameFolder::UpdateBooklist()
{
	FbFolderData * data = GetSelected();
	if (data) FillByFolder(data);
}

void FbFrameFolder::FillByFolder(FbFolderData * data)
{
	m_BooksPanel->SetFolder( data->GetId() );
	m_BooksPanel->SetType( data->GetType() );

	( new FolderThread(this, m_BooksPanel->GetListMode(), data) )->Execute();
}

void FbFrameFolder::OnFavoritesDel(wxCommandEvent & event)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	int iFolder = data->GetId();

	wxString selected = m_BooksPanel->m_BookList->GetSelected();
	wxString sql = wxString::Format(wxT("DELETE FROM favorites WHERE md5sum IN (SELECT books.md5sum FROM books WHERE id IN (%s)) AND id_folder=%d"), selected.c_str(), iFolder);

	FbCommonDatabase database;
	database.AttachConfig();
	database.ExecuteUpdate(sql);

	FbTreeListUpdater updater(m_BooksPanel->m_BookList);

	wxArrayInt items;
	m_BooksPanel->m_BookList->GetSelected(items);
	m_BooksPanel->m_BookList->DeleteItems(items);
}

void FbFrameFolder::OnFolderAppend(wxCommandEvent & event)
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

	FbMenuFolders::EmptyFolders();
	m_BooksPanel->EmptyBooks();
	FillFolders(id);
}

void FbFrameFolder::OnFolderModify(wxCommandEvent & event)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	if (data->GetType() != FT_FOLDER) return;
	int id = data->GetId();
	if (!id) return;

	wxTreeItemId item = m_MasterList->GetSelection();
	wxString name = m_MasterList->GetItemText(item);;
	name = wxGetTextFromUser(_("Введите новое имя папки:"), _("Изменить папку?"), name, this);
	if (name.IsEmpty()) return;

	FbLocalDatabase database;
	wxString sql = wxT("UPDATE folders SET value=? WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, name);
	stmt.Bind(2, id);
	stmt.ExecuteUpdate();

	FbMenuFolders::EmptyFolders();
	FillFolders(id);
}

void FbFrameFolder::OnFolderDelete(wxCommandEvent & event)
{
	FbFolderData * data = GetSelected();
	if (!data) return;
	if (data->GetType() != FT_FOLDER) return;
	int id = data->GetId();
	if (!id) return;

	wxTreeItemId item = m_MasterList->GetSelection();
	wxString name = m_MasterList->GetItemText(item);;
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

	FbMenuFolders::EmptyFolders();
	m_BooksPanel->EmptyBooks();
	FillFolders(0);
}

FbFolderData * FbFrameFolder::GetSelected()
{
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk())
		return (FbFolderData * ) m_MasterList->GetItemData(item);
	else
		return NULL;
}

void FbFrameFolder::UpdateFolder(const int iFolder, const FbFolderType type)
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

