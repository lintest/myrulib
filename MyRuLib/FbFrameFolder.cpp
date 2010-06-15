#include "FbFrameFolder.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbDownloader.h"

IMPLEMENT_CLASS(FbFolderParentData, FbParentData)

IMPLEMENT_CLASS(FbFolderChildData, FbChildData)

IMPLEMENT_CLASS(FbFrameFolder, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameFolder, FbFrameBase)
	EVT_MENU(ID_FAVORITES_DEL, FbFrameFolder::OnFavoritesDel)
	EVT_MENU(ID_APPEND_FOLDER, FbFrameFolder::OnFolderAppend)
	EVT_MENU(ID_MODIFY_FOLDER, FbFrameFolder::OnFolderModify)
	EVT_MENU(ID_DELETE_FOLDER, FbFrameFolder::OnFolderDelete)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameFolder::OnFolderSelected)
END_EVENT_TABLE()

FbFrameFolder::FbFrameFolder(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_FOLDER, GetTitle())
{
	CreateControls();
}

void FbFrameFolder::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar  = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, wxEmptyString);
	bSizer1->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbTreeViewCtrl(splitter, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxLC_VRULES);
	CreateColumns();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FillFolders();

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

wxToolBar * FbFrameFolder::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(FbParams::GetFont(FB_FONT_TOOL));
	toolbar->AddTool( ID_APPEND_FOLDER, _("Append"), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK));
	toolbar->AddTool( ID_MODIFY_FOLDER, _("Modify"), wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	toolbar->AddTool( ID_DELETE_FOLDER, _("Delete"), wxArtProvider::GetBitmap(wxART_DEL_BOOKMARK));
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_SAVE, _("Export"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Export to external device"));
	toolbar->Realize();

	return toolbar;
}

void FbFrameFolder::Localize(bool bUpdateMenu)
{
	FbFrameBase::Localize(bUpdateMenu);
	m_MasterList->EmptyColumns();
	CreateColumns();
}

void FbFrameFolder::CreateColumns()
{
	m_MasterList->AddColumn (0, _("Folders"), 100, wxALIGN_LEFT);
}

void FbFrameFolder::FillFolders(const int current)
{
	FbTreeModel * model = new FbTreeModel;

	FbParentData * root = new FbParentData(*model);
	m_folders = new FbFolderParentData(*model, root, _("Bookmarks"));
	new FbFolderChildData(*model, m_folders, 0, _("Favorites"), FT_FOLDER);
	model->SetRoot(root);

	try { 
		wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
		FbLocalDatabase database;
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		while (result.NextRow()) {
			int code = result.GetInt(0);
			wxString name = result.GetString(1);
			new FbFolderChildData(*model, m_folders, code, name, FT_FOLDER);
			if (code == current) model->FindRow(model->GetRowCount(), false);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	FbParentData * parent = new FbFolderParentData(*model, root, _("Remarks"));
	new FbFolderChildData(*model, parent, 1, _("Comments"), FT_COMMENT);
	for (int i=5; i>0; i--) new FbFolderChildData(*model, parent, i, GetRatingText(i), FT_RATING);
		
	m_MasterList->AssignModel(model);
}

void FbFrameFolder::OnFolderSelected(wxTreeEvent & event)
{
	m_BooksPanel->EmptyBooks();
	FbFolderChildData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbFolderChildData);
	if (data == NULL) return;

	bool enabled = data->GetType() == FT_FOLDER && data->GetCode();
	m_ToolBar->EnableTool(ID_MODIFY_FOLDER, enabled);
	m_ToolBar->EnableTool(ID_DELETE_FOLDER, enabled);
	FbMasterFolder(data->GetCode(), data->GetType()).Show(this);
}

void FbFrameFolder::UpdateBooklist()
{
	FbFolderChildData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbFolderChildData);
	if (data) FbMasterFolder(data->GetCode(), data->GetType()).Show(this);
}

void FbFrameFolder::OnFavoritesDel(wxCommandEvent & event)
{
	FbFolderChildData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbFolderChildData);
	if (data == NULL) return;
	
	int folder = data->GetCode();
	wxString selected = m_BooksPanel->m_BookList->GetSelected();
	wxString sql = wxString::Format(wxT("DELETE FROM favorites WHERE md5sum IN (SELECT books.md5sum FROM books WHERE id IN (%s)) AND id_folder=%d"), selected.c_str(), folder);

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
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	wxString name = wxGetTextFromUser(_("Input name of new folder:"), _("Add folder?"), wxEmptyString, this);
	if (name.IsEmpty()) return;
	int id = 0;

	try {
		FbLocalDatabase database;
		id = database.NewId(FB_NEW_FOLDER);
		wxString sql = wxT("INSERT INTO folders(value,id) VALUES(?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, name);
		stmt.Bind(2, id);
		stmt.ExecuteUpdate();
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return;
	}

	new FbFolderChildData(*model, m_folders, id, name, FT_FOLDER);
	model->FindRow(m_folders->Count(*model) + 1, true);
	OnFolderSelected(wxTreeEvent());
}

void FbFrameFolder::OnFolderModify(wxCommandEvent & event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbFolderChildData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbFolderChildData);
	if (data == NULL || data->GetType() != FT_FOLDER || data->GetCode()==0) return;

	wxString name = data->GetValue(*model);
	name = wxGetTextFromUser(_("Input new folder name:"), _("Change folder?"), name, this);
	if (name.IsEmpty()) return;

	try {
		FbLocalDatabase database;
		wxString sql = wxT("UPDATE folders SET value=? WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, name);
		stmt.Bind(2, data->GetCode());
		stmt.ExecuteUpdate();
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return;
	}

	data->SetName(name);
	m_MasterList->Refresh();
}

void FbFrameFolder::OnFolderDelete(wxCommandEvent & event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbFolderChildData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbFolderChildData);
	if (data == NULL || data->GetType() != FT_FOLDER || data->GetCode()==0) return;

	wxString name = data->GetValue(*model);
	wxString msg = wxString::Format(_("Delete folder \"%s\"?"), name.c_str());
	int answer = wxMessageBox(msg, _("Delete folder?"), wxOK | wxCANCEL, this);
	if (answer != wxOK) return;

	try {
		FbLocalDatabase database;
		wxString sql = wxT("DELETE FROM folders WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, data->GetCode());
		stmt.ExecuteUpdate();

		sql = wxT("DELETE FROM favorites WHERE id_folder=?");
		stmt = database.PrepareStatement(sql);
		stmt.Bind(1, data->GetCode());
		stmt.ExecuteUpdate();
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return;
	}
	m_MasterList->Delete();
}

void FbFrameFolder::UpdateFolder(const int folder, const FbFolderType type)
{
	FbFolderChildData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbFolderChildData);
	if (data == NULL || data->GetType() != type) return;

	bool update = false;
	switch (type) {
		case FT_FOLDER:
			update = data->GetCode() == folder;
			break;
		default:
			update = true;
			break;
	}
	if (update) FbMasterFolder(data->GetCode(), data->GetType()).Show(this);
}

void FbFrameFolder::ShowFullScreen(bool show)
{
	if (m_ToolBar) m_ToolBar->Show(!show);
	Layout();
}
