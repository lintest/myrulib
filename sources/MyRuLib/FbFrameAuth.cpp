#include "FbFrameAuth.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbClientData.h"
#include "FbExportDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbAuthorDlg.h"
#include "FbMasterTypes.h"
#include "controls/FbToolBar.h"

IMPLEMENT_CLASS(FbFrameAuth, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameAuth, FbFrameBase)
	EVT_COMBOBOX( ID_INIT_LETTER, FbFrameAuth::OnChoiceLetter )
	EVT_COMBOBOX( ID_CHOICE_LETTER, FbFrameAuth::OnChoiceLetter )
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameAuth::OnColClick)
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameAuth::OnContextMenu)
	EVT_MENU(ID_MASTER_APPEND, FbFrameAuth::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbFrameAuth::OnMasterModify)
	EVT_MENU(ID_MASTER_REPLACE, FbFrameAuth::OnMasterReplace)
	EVT_MENU(ID_MASTER_DELETE, FbFrameAuth::OnMasterDelete)
	EVT_MENU(ID_MASTER_PAGE, FbFrameAuth::OnMasterPage)
	EVT_UPDATE_UI(ID_MASTER_PAGE, FbFrameAuth::OnMasterPageUpdateUI)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameAuth::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameAuth::OnArray)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameAuth::OnBooksCount)
END_EVENT_TABLE()

FbFrameAuth::FbFrameAuth(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_AUTH, GetTitle())
{
	CreateControls();
}

void FbFrameAuth::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	wxPanel * panel = new wxPanel( splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer * bsMasterList = new wxBoxSizer( wxVERTICAL );

	m_LetterList = new FbAlphabetCombo();
	m_LetterList->Create(panel, ID_CHOICE_LETTER, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCB_READONLY);
	bsMasterList->Add( m_LetterList, 0, wxEXPAND, 0 );

	m_MasterList = new FbTreeViewCtrl(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_MasterList->SetSortedColumn(1);
	CreateColumns();
	bsMasterList->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );

	panel->SetSizer( bsMasterList );
	panel->Layout();
	bsMasterList->Fit( panel );

	CreateBooksPanel(splitter);
	splitter->SplitVertically(panel, m_BooksPanel, 160);

	FbFrameBase::CreateControls();
}

void FbFrameAuth::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Author"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

void FbFrameAuth::OnChoiceLetter(wxCommandEvent& event)
{
	int selection = m_LetterList->GetSelection();
	if (selection == wxNOT_FOUND) return;
	if (event.GetId() == ID_INIT_LETTER && m_info) return;
	wxString letter = m_LetterList->GetString(selection).Left(1);
	if (letter.IsEmpty()) return;
	FbParams::Set(FB_LAST_LETTER, letter);
	m_info = (wxChar)letter[0];
	CreateMasterThread();
	m_LetterList->SetText();
}

void FbFrameAuth::CreateMasterThread()
{
	m_MasterList->AssignModel(NULL);
	if (m_MasterThread) m_MasterThread->Wait();
	wxDELETE(m_MasterThread);
	m_MasterThread = new FbAuthListThread(this, m_info, m_MasterList->GetSortedColumn());
	m_MasterThread->Execute();
}

void FbFrameAuth::UpdateFonts(bool refresh)
{
	m_LetterList->SetFont(FbParams::GetFont(FB_FONT_MAIN));
	FbFrameBase::UpdateFonts(refresh);
}

void FbFrameAuth::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameAuth::FindAuthor(const wxString &text)
{
	if (text.IsEmpty()) return;
	m_info = text;
	CreateMasterThread();
	m_LetterList->SetText(text);
}

void FbFrameAuth::OnColClick(wxListEvent& event)
{
	CreateMasterThread();
}

void FbFrameAuth::OnBooksCount(FbCountEvent& event)
{
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	FbMasterAuthInfo * info = wxDynamicCast(&event.GetInfo(), FbMasterAuthInfo);
	if (model && info) {
		model->SetCount(info->GetId(), event.GetCount());
		m_MasterList->Refresh();
	}

	event.Skip();
}

void FbFrameAuth::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameAuth::ShowContextMenu(const wxPoint& pos, wxTreeItemId)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
	int id = data ? data->GetCode() : 0;
	MasterMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameAuth::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbAuthorModifyDlg::Append(newname);
	if (id == 0) return;
	m_MasterList->Append(new FbAuthListData(id));
}

void FbFrameAuth::OnMasterModify(wxCommandEvent& event)
{
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model == NULL) return;

	FbModelItem item = m_MasterList->GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbAuthorModifyDlg::Modify(old_id, newname);
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	m_MasterList->Replace(new FbAuthListData(new_id));
}

void FbFrameAuth::OnMasterReplace(wxCommandEvent& event)
{
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model == NULL) return;

	FbModelItem item = m_MasterList->GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbAuthorReplaceDlg::Execute(old_id, newname);
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	m_MasterList->Replace(new FbAuthListData(new_id));
}

void FbFrameAuth::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model) {
		bool ok = FbAuthorReplaceDlg::Delete(*model);
		if (ok) m_MasterList->Delete();
	}
}

void FbFrameAuth::OnMasterPage(wxCommandEvent& event)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
	if (data && data->GetCode() > 0) {
		wxString host = FbParams::GetStr(DB_DOWNLOAD_HOST);
		wxString url = wxString::Format(wxT("http://%s/a/%d"), host.c_str(), data->GetCode());
		wxLaunchDefaultBrowser(url);
	}
}

FbFrameAuth::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_REPLACE, _("Replace"));
	Append(ID_MASTER_DELETE,  _("Delete"));
	if (id > 0) {
		AppendSeparator();
		Append(ID_MASTER_PAGE, _("Online authors page"));
	}
}

FbFrameAuth::MenuBar::MenuBar()
{
	Append(new MenuFile,   _("&File"));
	Append(new MenuLib,    _("&Library"));
	Append(new MenuFrame,  _("&Catalog"));
	Append(new MenuMaster, _("&Authors"));
	Append(new MenuBook,   _("&Books"));
	Append(new MenuView,   _("&View"));
	Append(new MenuSetup,  _("&Tools"));
	Append(new MenuWindow, _("&Window"));
	Append(new MenuHelp,   _("&?"));
}

FbFrameAuth::MenuMaster::MenuMaster()
{
	Append(ID_MASTER_APPEND,  _("Append"));
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_REPLACE, _("Replace"));
	Append(ID_MASTER_DELETE,  _("Delete"));
	AppendSeparator();
	Append(ID_MASTER_PAGE, _("Online authors page"));
}

wxMenuBar * FbFrameAuth::CreateMenuBar()
{
	return new MenuBar;
}

void FbFrameAuth::OnMasterPageUpdateUI(wxUpdateUIEvent & event)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
	event.Enable( data && data->GetCode()>0 );
}

void FbFrameAuth::OnModel( FbArrayEvent& event )
{
	FbAuthListModel * model = new FbAuthListModel(event.GetArray());
	m_MasterList->AssignModel(model);
}

void FbFrameAuth::OnArray( FbArrayEvent& event )
{
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}

