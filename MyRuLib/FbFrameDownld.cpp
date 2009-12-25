#include "FbFrameDownld.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "FbDownloader.h"
#include "FbUpdateThread.h"
#include "res/start.xpm"
#include "res/pause.xpm"

BEGIN_EVENT_TABLE(FbFrameDownld, FbFrameBase)
	EVT_MENU(ID_START, FbFrameDownld::OnStart)
	EVT_MENU(ID_PAUSE, FbFrameDownld::OnPause)
	EVT_MENU(wxID_UP, FbFrameDownld::OnMoveUp)
	EVT_MENU(wxID_DOWN, FbFrameDownld::OnMoveDown)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameDownld::OnFolderSelected)
	EVT_MENU(ID_DELETE_DOWNLOAD, FbFrameDownld::OnSubmenu)
END_EVENT_TABLE()

FbFrameDownld::FbFrameDownld(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_DOWNLD, _("Загрузки"))
{
	CreateControls();
}

void FbFrameDownld::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, wxEmptyString);
	bSizer1->Add( m_ToolBar, 0, wxGROW);

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

wxToolBar * FbFrameDownld::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(FbParams::GetFont(FB_FONT_TOOL));

	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_START, _("Старт"), wxBitmap(start_xpm), _("Начать загрузку файлов через интернет"));
	toolbar->AddTool(ID_PAUSE, _("Стоп"), wxBitmap(pause_xpm), _("Остановить загрузку файлов через интернет"));
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_UP, _("Вверх"), wxArtProvider::GetBitmap(wxART_GO_UP), _("Передвинуть в начало очереди"));
	toolbar->AddTool(wxID_DOWN, _("Вниз"), wxArtProvider::GetBitmap(wxART_GO_DOWN), _("Передвинуть в конец очереди"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_DELETE_DOWNLOAD, _("Удалить"), wxArtProvider::GetBitmap(wxART_DELETE), _("Удалить загрузку"));
	toolbar->Realize();

	return toolbar;
}

void FbFrameDownld::FillFolders(const int iCurrent)
{
	m_MasterList->Freeze();
	m_MasterList->DeleteRoot();

	wxTreeItemId root = m_MasterList->AddRoot(wxEmptyString);
	m_MasterList->AppendItem(root, wxT("Очередь"), -1, -1, new FbFolderData(1, FT_DOWNLOAD));
	m_MasterList->AppendItem(root, wxT("Готово"), -1, -1, new FbFolderData(-1, FT_DOWNLOAD));
	m_MasterList->AppendItem(root, wxT("Ошибки"), -1, -1, new FbFolderData(-2, FT_DOWNLOAD));
	m_MasterList->Expand(root);

	m_MasterList->Thaw();
}

FbThreadSkiper FbFrameDownld::DownldThread::sm_skiper;

wxString FbFrameDownld::DownldThread::GetOrder()
{
	if (m_folder==1) return wxT("download");
	return BaseThread::GetOrder();
}

void * FbFrameDownld::DownldThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition;
	condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE download");
	condition += ( m_folder==1 ? wxT(">=?)") : wxT("=?)") );

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

void FbFrameDownld::OnFolderSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbFolderData * data = (FbFolderData*) m_MasterList->GetItemData(selected);
		if (data) {
			bool enabled = data->GetId() > 0;
			m_ToolBar->EnableTool(wxID_UP,   enabled);
			m_ToolBar->EnableTool(wxID_DOWN, enabled);
			FillByFolder(data);
		}
	}
}

void FbFrameDownld::UpdateBooklist()
{
	FbFolderData * data = GetSelected();
	if (data) FillByFolder(data);
}

void FbFrameDownld::FillByFolder(FbFolderData * data)
{
	m_BooksPanel->SetFolder( data->GetId() );
	m_BooksPanel->SetType( FT_DOWNLOAD );
	( new DownldThread(this, m_BooksPanel->GetListMode(), data) )->Execute();
}

FbFolderData * FbFrameDownld::GetSelected()
{
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk())
		return (FbFolderData * ) m_MasterList->GetItemData(item);
	else
		return NULL;
}

void FbFrameDownld::UpdateFolder(const int iFolder, const FbFolderType type)
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

void FbFrameDownld::OnStart(wxCommandEvent & event)
{
	FbDownloader::Start();
}

void FbFrameDownld::OnPause(wxCommandEvent & event)
{
	FbDownloader::Pause();
}

void FbFrameDownld::OnMoveUp(wxCommandEvent& event)
{
	wxString sel = m_BooksPanel->m_BookList->GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download+1 WHERE download>0 AND md5sum NOT IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD) )->Execute();
}

void FbFrameDownld::OnMoveDown(wxCommandEvent& event)
{
	wxString sel = m_BooksPanel->m_BookList->GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download+1 WHERE download>0 AND md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD))->Execute();
}

