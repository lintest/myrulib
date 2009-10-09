#include "FbFrameFavour.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "BooksPanel.h"
#include "FbGenres.h"
#include "FbClientData.h"
#include "MyRuLibApp.h"
#include "FbFrameBaseThread.h"

BEGIN_EVENT_TABLE(FbFrameFavour, FbFrameBase)
	EVT_MENU(ID_FAVORITES_DEL, FbFrameFavour::OnFavoritesDel)
    EVT_MENU(ID_APPEND_FOLDER, FbFrameFavour::OnFolderAppend)
    EVT_MENU(ID_MODIFY_FOLDER, FbFrameFavour::OnFolderModify)
    EVT_MENU(ID_DELETE_FOLDER, FbFrameFavour::OnFolderDelete)
    EVT_LISTBOX(ID_FOLDER_LIST, FbFrameFavour::OnFolderSelected)
END_EVENT_TABLE()

FbFrameFavour::FbFrameFavour(wxAuiMDIParentFrame * parent, wxWindowID id,const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameFavour::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	SetMenuBar(CreateMenuBar());

	wxBoxSizer* bToolSizer = new wxBoxSizer( wxHORIZONTAL );

	wxToolBar * m_tools = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS|wxTB_FLAT );
	m_tools->AddTool( ID_APPEND_FOLDER, _("Добавить"), wxNullBitmap);
	m_tools->AddTool( ID_MODIFY_FOLDER, _("Изменить"), wxNullBitmap);
	m_tools->AddTool( ID_DELETE_FOLDER, _("Удалить"), wxNullBitmap);
	m_tools->Realize();
	bToolSizer->Add( m_tools, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL);

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bToolSizer->Add( toolbar, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL);

	bSizer1->Add( bToolSizer, 0, wxEXPAND);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	m_FolderList = new wxListBox(splitter, ID_FOLDER_LIST);
	FillFolders();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BooksPanel.Create(splitter, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxNO_BORDER, substyle);
	splitter->SplitVertically(m_FolderList, &m_BooksPanel, 160);

    m_BooksPanel.CreateColumns(GetListMode(FB_MODE_FAVOUR));

	SetSizer( bSizer1 );
	Layout();

	FillByFolder(0);
}

wxToolBar * FbFrameFavour::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
    wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();
    return toolbar;
}

void FbFrameFavour::FillFolders(const int iCurrent)
{
	m_FolderList->Freeze();
	m_FolderList->Clear();

    m_FolderList->Append(_("Избранное"), new FbClientData(0));
	if (iCurrent == 0) m_FolderList->SetSelection(0);

    wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
    wxSQLite3ResultSet result = wxGetApp().GetConfigDatabase().ExecuteQuery(sql);
    while (result.NextRow()) {
        int id = result.GetInt(0);
        wxString name = result.GetString(1);
        m_FolderList->Append(name, new FbClientData(id));
        if (id == iCurrent) m_FolderList->SetSelection(m_FolderList->GetCount() - 1);
    }

	m_FolderList->Thaw();
}

class FrameFavourThread: public FbFrameBaseThread
{
    public:
        FrameFavourThread(FbFrameBase * frame, FbListMode mode, const int folder)
			:FbFrameBaseThread(frame, mode), m_folder(folder), m_number(sm_skiper.NewNumber()) {};
        virtual void *Entry();
    private:
		static FbThreadSkiper sm_skiper;
        int m_folder;
        int m_number;
};

FbThreadSkiper FrameFavourThread::sm_skiper;

void * FrameFavourThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition = wxT("md5sum IN (SELECT DISTINCT md5sum FROM favorites WHERE id_folder = ?)");
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		database.AttachConfig();
		FbGenreFunction function;
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

void FbFrameFavour::OnFolderSelected(wxCommandEvent & event)
{
	FbClientData * data = (FbClientData*)event.GetClientObject();
	if (data) FillByFolder(data->GetID());
}

void FbFrameFavour::UpdateBooklist()
{
    int iSelected = m_FolderList->GetSelection();
    if (iSelected == wxNOT_FOUND) return;

    FbClientData * data = (FbClientData*) m_FolderList->GetClientObject(iSelected);
	if (data) FillByFolder(data->GetID());
}

void FbFrameFavour::FillByFolder(const int iFolder)
{
	m_BooksPanel.SetFolder(iFolder);
	wxThread * thread = new FrameFavourThread(this, m_BooksPanel.GetListMode(), iFolder);
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbFrameFavour::OnFavoritesDel(wxCommandEvent & event)
{
    int iSelected = m_FolderList->GetSelection();
    if (iSelected == wxNOT_FOUND) return;
    FbClientData * data = (FbClientData*) m_FolderList->GetClientObject(iSelected);
	if (!data) return;
	int iFolder = data->GetID();

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

    m_BooksPanel.EmptyBooks();

	FbConfigDatabase & database = wxGetApp().GetConfigDatabase();
	int id = database.NewId(FB_NEW_FOLDER);
    wxString sql = wxT("INSERT INTO folders(value,id) VALUES(?,?)");
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, name);
    stmt.Bind(2, id);
    stmt.ExecuteUpdate();

    FbBookMenu::EmptyFolders();
    FillFolders(id);
}

void FbFrameFavour::OnFolderModify(wxCommandEvent & event)
{
    int iSelected = m_FolderList->GetSelection();
    if (iSelected <= 0) return;

    wxString name = m_FolderList->GetStringSelection();
	name = wxGetTextFromUser(_("Введите новое имя папки:"), _("Изменить папку?"), name, this);
	if (name.IsEmpty()) return;

    FbClientData * data = (FbClientData*) m_FolderList->GetClientObject(iSelected);
	if (!data) return;

	int id = data->GetID();
	FbConfigDatabase & database = wxGetApp().GetConfigDatabase();
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
    int iSelected = m_FolderList->GetSelection();
    if (iSelected <= 0) return;

    wxString name = m_FolderList->GetStringSelection();
    wxString msg = wxString::Format(_("Удалить папку «%s»?"), name.c_str());
	int answer = wxMessageBox(msg, _("Удалить папку?"), wxOK | wxCANCEL, this);
	if (answer != wxOK) return;

    FbClientData * data = (FbClientData*) m_FolderList->GetClientObject(iSelected);
	if (!data) return;

	int id = data->GetID();
	FbConfigDatabase & database = wxGetApp().GetConfigDatabase();
    wxString sql = wxT("DELETE FROM folders WHERE id=?");
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, id);
    stmt.ExecuteUpdate();

    sql = wxT("DELETE FROM favorites WHERE id_folder=?");
    stmt = database.PrepareStatement(sql);
    stmt.Bind(1, id);
    stmt.ExecuteUpdate();

    m_FolderList->Delete(iSelected);

    FbBookMenu::EmptyFolders();
    m_BooksPanel.EmptyBooks();
}

void FbFrameFavour::UpdateFolder(const int iFolder)
{
    int iSelected = m_FolderList->GetSelection();
    if (iSelected == wxNOT_FOUND) return;
    FbClientData * data = (FbClientData*) m_FolderList->GetClientObject(iSelected);
	if (data && data->GetID()==iFolder) FillByFolder(iFolder);
}

