#include "FbFrameFldr.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbFldrTree.h"
#include "FbDatabase.h"
#include "FbDownloader.h"

//-----------------------------------------------------------------------------
//  FbFrameFldr
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFrameFldr, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameFldr, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameFldr::OnFolderSelected)
	EVT_MENU(ID_FAVORITES_DEL, FbFrameFldr::OnFavoritesDel)
	EVT_MENU(ID_APPEND_FOLDER, FbFrameFldr::OnFolderAppend)
	EVT_MENU(ID_MODIFY_FOLDER, FbFrameFldr::OnFolderModify)
	EVT_MENU(ID_DELETE_FOLDER, FbFrameFldr::OnFolderDelete)
END_EVENT_TABLE()

FbFrameFldr::FbFrameFldr(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_FLDR, GetTitle())
{
	CreateControls();
}

void FbFrameFldr::CreateControls()
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	wxPanel * panel = new wxPanel( splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer * bsMasterList = new wxBoxSizer( wxVERTICAL );

	m_ToolBar.Create(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_NODIVIDER);
	m_ToolBar.SetFont(FbParams::GetFont(FB_FONT_TOOL));
	m_ToolBar.AddTool( ID_APPEND_FOLDER, _("Append"), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK));
	m_ToolBar.AddTool( ID_MODIFY_FOLDER, _("Modify"), wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	m_ToolBar.AddTool( ID_DELETE_FOLDER, _("Delete"), wxArtProvider::GetBitmap(wxART_DEL_BOOKMARK));
	m_ToolBar.Realize();
	bsMasterList->Add( &m_ToolBar, 0, wxEXPAND, 0 );

	m_MasterList = new FbTreeViewCtrl(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	CreateColumns();
	bsMasterList->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );

	panel->SetSizer( bsMasterList );
	panel->Layout();
	bsMasterList->Fit( panel );

	CreateBooksPanel(splitter);
	splitter->SplitVertically(panel, m_BooksPanel, 160);

	SetSizer( sizer );

	FbFrameBase::CreateControls();
	FillFolders();
}

void FbFrameFldr::CreateColumns()
{
	m_MasterList->AddColumn (0, _("Folders"), 100, wxALIGN_LEFT);
}

void FbFrameFldr::FillFolders(const int current)
{
	FbTreeModel * model = new FbTreeModel;

	FbParentData * root = new FbParentData(*model);
	m_folders = new FbFolderParentData(*model, root, _("Bookmarks"));
	new FbFolderChildData(*model, m_folders, 0, _("Favorites"));
	model->SetRoot(root);

	wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		int code = result.GetInt(0);
		wxString name = result.GetString(1);
		new FbFolderChildData(*model, m_folders, code, name);
		if (code == current) model->FindRow(model->GetRowCount(), false);
	}

	FbParentData * parent = new FbFolderParentData(*model, root, _("Remarks"));
	new FbCommChildData(*model, parent);
	for (int i=5; i>0; i--) new FbRateChildData(*model, parent, i);

	model->GoNextRow();
	m_MasterList->AssignModel(model);
}

void FbFrameFldr::OnFolderSelected(wxTreeEvent & event)
{
	UpdateBooklist();

	FbModelItem item = m_MasterList->GetCurrent();
	FbFolderChildData * data = wxDynamicCast(&item, FbFolderChildData);
	bool enabled = data && data->GetCode();
	m_ToolBar.EnableTool(ID_MODIFY_FOLDER, enabled);
	m_ToolBar.EnableTool(ID_DELETE_FOLDER, enabled);
}

void FbFrameFldr::OnFavoritesDel(wxCommandEvent & event)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbFolderChildData * data = wxDynamicCast(&item, FbFolderChildData);
	if (data == NULL) return;

	int folder = data->GetCode();
	wxString selected = m_BooksPanel->GetSelected();
	wxString sql = wxString::Format(wxT("DELETE FROM favorites WHERE md5sum IN (SELECT md5sum FROM books WHERE id IN (%s)) AND id_folder=%d"), selected.c_str(), folder);

	FbCommonDatabase database;
	database.AttachConfig();
	database.ExecuteUpdate(sql);

	m_BooksPanel->m_BookList->Delete();
}

void FbFrameFldr::OnFolderAppend(wxCommandEvent & event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	wxString name = wxGetTextFromUser(_("Input name of new folder:"), _("Add folder?"), wxEmptyString, this);
	if (name.IsEmpty()) return;
	int id = 0;

	FbLocalDatabase database;
	id = database.NewId(FB_NEW_FOLDER);
	wxString sql = wxT("INSERT INTO folders(value,id) VALUES(?,?)");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, name);
	stmt.Bind(2, id);
	bool ok = stmt.ExecuteUpdate();

	if (ok) {
		new FbFolderChildData(*model, m_folders, id, name);
		model->FindRow(m_folders->Count(*model) + 1, true);
		wxTreeEvent treeEvent;
		OnFolderSelected(treeEvent);
	}
}

void FbFrameFldr::OnFolderModify(wxCommandEvent & event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbModelItem item = m_MasterList->GetCurrent();
	FbFolderChildData * data = wxDynamicCast(&item, FbFolderChildData);
	if (data == NULL || data->GetCode() == 0) return;

	wxString name = data->GetValue(*model);
	name = wxGetTextFromUser(_("Input new folder name:"), _("Change folder?"), name, this);
	if (name.IsEmpty()) return;

	FbLocalDatabase database;
	wxString sql = wxT("UPDATE folders SET value=? WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, name);
	stmt.Bind(2, data->GetCode());
	bool ok = stmt.ExecuteUpdate();

	if (ok) {
		data->SetName(name);
		m_MasterList->Refresh();
	}
}

void FbFrameFldr::OnFolderDelete(wxCommandEvent & event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbModelItem item = m_MasterList->GetCurrent();
	FbFolderChildData * data = wxDynamicCast(&item, FbFolderChildData);
	if (data == NULL || data->GetCode() == 0) return;

	wxString name = data->GetValue(*model);
	wxString msg = wxString::Format(_("Delete folder \"%s\"?"), name.c_str());
	int answer = wxMessageBox(msg, _("Delete folder?"), wxOK | wxCANCEL, this);
	if (answer != wxOK) return;

	FbLocalDatabase database;
	wxString sql = wxT("DELETE FROM folders WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, data->GetCode());
	bool ok = stmt.ExecuteUpdate();

	sql = wxT("DELETE FROM favorites WHERE id_folder=?");
	stmt = database.PrepareStatement(sql);
	stmt.Bind(1, data->GetCode());
	stmt.ExecuteUpdate();

	if (ok) m_MasterList->Delete();
}

void FbFrameFldr::UpdateFolder(const int folder, const FbFolderType type)
{
	FbModelItem item = m_MasterList->GetCurrent();

	bool update = false;
	switch (type) {
		case FT_FOLDER: {
			FbFolderChildData * data = wxDynamicCast(&item, FbFolderChildData);
			update = data && data->GetCode() == folder;
		} break;
		case FT_COMMENT: {
			update = wxDynamicCast(&item, FbCommChildData);
		} break;
		case FT_RATING: {
			update = wxDynamicCast(&item, FbRateChildData);
		} break;
		default: {
			update = false;
		} break;
	}
	if (update) UpdateBooklist();
}

