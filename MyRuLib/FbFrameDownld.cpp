#include "FbFrameDownld.h"
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

BEGIN_EVENT_TABLE(FbFrameDownld, FbFrameBase)
	EVT_MENU(ID_START, FbFrameDownld::OnStart)
	EVT_MENU(ID_PAUSE, FbFrameDownld::OnPause)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameDownld::OnFolderSelected)
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

	m_toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, wxEmptyString);
	bSizer1->Add( m_toolbar, 0, wxGROW);

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

wxToolBar * FbFrameDownld::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_START, _("Старт"), wxBitmap(start_xpm), _("Начать загрузку файлов через интернет"));
	toolbar->AddTool(ID_PAUSE, _("Стоп"), wxBitmap(pause_xpm), _("Остановить загрузку файлов через интернет"));
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_UP, _("Вверх"), wxArtProvider::GetBitmap(wxART_GO_UP), _("Передвинуть в начало очереди"));
	toolbar->AddTool(wxID_DOWN, _("Вниз"), wxArtProvider::GetBitmap(wxART_GO_DOWN), _("Передвинуть в конец очереди"));
	toolbar->Realize();
	return toolbar;
}

void FbFrameDownld::FillFolders(const int iCurrent)
{
	m_FolderList->Freeze();
	m_FolderList->DeleteRoot();

	wxTreeItemId root = m_FolderList->AddRoot(wxEmptyString);
	m_FolderList->AppendItem(root, wxT("Очередь"), -1, -1, new FbFolderData(1, FT_DOWNLOAD));
	m_FolderList->AppendItem(root, wxT("Готово"), -1, -1, new FbFolderData(-1, FT_DOWNLOAD));
	m_FolderList->AppendItem(root, wxT("Ошибки"), -1, -1, new FbFolderData(-2, FT_DOWNLOAD));
	m_FolderList->Expand(root);

	m_FolderList->Thaw();
}

class FrameDownldThread: public FbFrameBaseThread
{
	public:
		FrameDownldThread(FbFrameBase * frame, FbListMode mode, FbFolderData * data)
			:FbFrameBaseThread(frame, mode), m_folder(data->GetId()), m_number(sm_skiper.NewNumber()), m_type(data->GetType()) {};
		virtual void *Entry();
	private:
		static FbThreadSkiper sm_skiper;
		int m_folder;
		int m_number;
		FbFolderType m_type;
};

FbThreadSkiper FrameDownldThread::sm_skiper;

void * FrameDownldThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition;
	condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE download");
	condition += ( m_folder==1 ? wxT(">=?)") : wxT("=?)") );
	wxString order = m_folder==1 ? wxT("download") : wxEmptyString;

	wxString sql = GetSQL(condition, order);

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

void FbFrameDownld::OnFolderSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel.EmptyBooks();
		FbFolderData * data = (FbFolderData*) m_FolderList->GetItemData(selected);
		if (data) {
			bool enabled = data->GetId() > 0;
			m_toolbar->EnableTool(wxID_UP,   enabled);
			m_toolbar->EnableTool(wxID_DOWN, enabled);
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
	m_BooksPanel.SetFolder( data->GetId() );
	m_BooksPanel.SetType( FT_DOWNLOAD );

	wxThread * thread = new FrameDownldThread(this, m_BooksPanel.GetListMode(), data);
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

FbFolderData * FbFrameDownld::GetSelected()
{
	wxTreeItemId item = m_FolderList->GetSelection();
	if (item.IsOk())
		return (FbFolderData * ) m_FolderList->GetItemData(item);
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
