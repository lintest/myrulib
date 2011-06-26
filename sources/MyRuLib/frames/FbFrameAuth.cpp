#include "FbFrameAuth.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbClientData.h"
#include "dialogs/FbExportDlg.h"
#include "dialogs/FbAuthorDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbMasterTypes.h"
#include "controls/FbToolBar.h"

//-----------------------------------------------------------------------------
//  FbAuthViewCtrl
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbAuthViewCtrl, FbMasterViewCtrl)
	EVT_MENU(wxID_DELETE, FbAuthViewCtrl::OnMasterDelete)
	EVT_MENU(ID_MASTER_REPLACE, FbAuthViewCtrl::OnMasterReplace)
	EVT_MENU(ID_MASTER_APPEND, FbAuthViewCtrl::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbAuthViewCtrl::OnMasterModify)
	EVT_MENU(ID_MASTER_PAGE, FbAuthViewCtrl::OnMasterPage)
	EVT_UPDATE_UI(ID_MASTER_REPLACE, FbMasterViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(ID_MASTER_APPEND, FbMasterViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(ID_MASTER_MODIFY, FbMasterViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(ID_MASTER_PAGE, FbAuthViewCtrl::OnMasterPageUpdateUI)
END_EVENT_TABLE()

void FbAuthViewCtrl::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbAuthorModifyDlg::Append(newname);
	if (id == 0) return;
	Append(new FbAuthListData(id));
}

void FbAuthViewCtrl::OnMasterModify(wxCommandEvent& event)
{
	FbAuthListModel * model = wxDynamicCast(GetModel(), FbAuthListModel);
	if (model == NULL) return;

	FbModelItem item = GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbAuthorModifyDlg::Modify(old_id, newname);
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	Replace(new FbAuthListData(new_id));
}

void FbAuthViewCtrl::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = GetModel();
	if (model) {
		bool ok = FbAuthorReplaceDlg::Delete(*model);
		if (ok) Delete();
	}
}

void FbAuthViewCtrl::OnMasterPage(wxCommandEvent& event)
{
	FbModelItem item = GetCurrent();
	FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
	if (data && data->GetCode() > 0) {
		wxString host = FbParams(DB_DOWNLOAD_HOST);
		wxString url = wxString::Format(wxT("http://%s/a/%d"), host.c_str(), data->GetCode());
		wxLaunchDefaultBrowser(url);
	}
}

void FbAuthViewCtrl::OnMasterPageUpdateUI(wxUpdateUIEvent & event)
{
	FbModelItem item = GetCurrent();
	FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
	event.Enable( data && data->GetCode()>0 );
}

void FbAuthViewCtrl::OnMasterReplace(wxCommandEvent& event)
{
	FbAuthListModel * model = wxDynamicCast(GetModel(), FbAuthListModel);
	if (model == NULL) return;

	FbModelItem item = GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	if (current == NULL) return;

	FbFrameBase * frame = wxDynamicCast(GetParent()->GetParent(), FbFrameBase);
	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbAuthorReplaceDlg::Execute(old_id, newname, frame->GetMasterFile());
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	Replace(new FbAuthListData(new_id));
}

//-----------------------------------------------------------------------------
//  FbFrameAuth
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFrameAuth, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameAuth, FbFrameBase)
	EVT_COMBOBOX( ID_INIT_LETTER, FbFrameAuth::OnChoiceLetter )
	EVT_COMBOBOX( ID_MASTER_FIND, FbFrameAuth::OnChoiceLetter )
	EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameAuth::OnColClick)
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameAuth::OnContextMenu)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameAuth::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameAuth::OnArray)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameAuth::OnBooksCount)
	EVT_COMMAND(ID_MODEL_NUMBER, fbEVT_BOOK_ACTION, FbFrameAuth::OnNumber)
END_EVENT_TABLE()

FbFrameAuth::FbFrameAuth(wxAuiNotebook * parent, bool select)
	: FbFrameBase(parent, ID_FRAME_AUTH, GetTitle(), select)

{
	wxPanel * panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_LetterList = new FbAlphabetCombo();
	m_LetterList->Create(panel, ID_MASTER_FIND, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCB_READONLY);

	m_MasterList = new FbAuthViewCtrl;
	m_MasterList->Create(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_MasterList->SetSortedColumn(1);
	CreateColumns();

	CreateBooksPanel(this);

	sizer->Add( m_LetterList, 0, wxEXPAND, 0 );
	sizer->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );
	panel->SetSizer( sizer );
	panel->Layout();
	sizer->Fit( panel );

	SplitVertically(panel, m_BooksPanel);

	CreateControls(select);
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
	FbParams(FB_LAST_LETTER) = letter;
	m_info = (wxChar)letter[0];
	CreateMasterThread();
	m_LetterList->SetText();
}

void FbFrameAuth::CreateMasterThread()
{
	m_MasterList->AssignModel(NULL);
	if (m_MasterThread) {
		m_MasterThread->Close();
		m_MasterThread->Wait();
	}
	wxDELETE(m_MasterThread);

	m_MasterThread = new FbAuthListThread(this, m_info, m_MasterList->GetSortedColumn(), m_MasterFile);
	m_MasterThread->Execute();
}

void FbFrameAuth::UpdateFonts(bool refresh)
{
	m_LetterList->SetFont(FbParams(FB_FONT_MAIN));
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
	m_MasterList->PopupMenu(&menu, pos.x, pos.y);
}

FbFrameAuth::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_REPLACE, _("Replace"));
	Append(wxID_DELETE,       _("Delete"));
	if (id > 0) {
		AppendSeparator();
		Append(ID_MASTER_PAGE, _("Online authors page"));
	}
}

void FbFrameAuth::OnModel( FbArrayEvent& event )
{
	FbAuthListModel * model = new FbAuthListModel(event.GetArray());
	model->SetCounter(m_MasterFile);
	m_MasterList->AssignModel(model);
}

void FbFrameAuth::OnArray( FbArrayEvent& event )
{
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}

void FbFrameAuth::OnNumber(wxCommandEvent& event)
{
	m_MasterFile = event.GetString();
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model) model->SetCounter(m_MasterFile);
	m_MasterList->Refresh();
}
