#include "FbFrameSequen.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbMainMenu.h"
#include "FbUpdateThread.h"
#include "FbMasterList.h"
#include "FbWindow.h"
#include "FbParams.h"
#include "FbSequenDlg.h"

IMPLEMENT_CLASS(FbFrameSequen, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameSequen, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameSequen::OnMasterSelected)
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameSequen::OnColClick)
	EVT_COMMAND(ID_BOOKS_COUNT, fbEVT_BOOK_ACTION, FbFrameSequen::OnBooksCount)
	EVT_TEXT_ENTER(ID_SEQUENCE_TXT, FbFrameSequen::OnFindEnter )
	EVT_MENU(ID_SEQUENCE_BTN, FbFrameSequen::OnFindEnter )
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameSequen::OnContextMenu)
	EVT_MENU(ID_MASTER_APPEND, FbFrameSequen::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbFrameSequen::OnMasterModify)
	EVT_MENU(ID_MASTER_DELETE, FbFrameSequen::OnMasterDelete)
	EVT_COMMAND(ID_APPEND_AUTHOR, fbEVT_BOOK_ACTION, FbFrameSequen::OnAppendAuthor)
	EVT_COMMAND(ID_APPEND_SEQUENCE, fbEVT_BOOK_ACTION, FbFrameSequen::OnAppendSequence)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameSequen::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameSequen::OnArray)
END_EVENT_TABLE()

FbFrameSequen::FbFrameSequen(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_SEQUEN, GetTitle()), m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	CreateControls();
}

void FbFrameSequen::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	m_ToolBar = CreateToolBar();
	sizer->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbTreeViewCtrl(splitter, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_MasterList->SetSortedColumn(1);
	CreateColumns();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	FindSequence(wxEmptyString);
}

void FbFrameSequen::Localize(bool bUpdateMenu)
{
	m_MasterList->EmptyColumns();
	CreateColumns();
	FbFrameBase::Localize(bUpdateMenu);
}

void FbFrameSequen::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Ser."), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

wxToolBar * FbFrameSequen::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxFont font = FbParams::GetFont(FB_FONT_TOOL);

	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(font);

	m_FindInfo = new wxStaticText( toolbar, wxID_ANY, (wxString)_("Ser.") + wxT(':'), wxDefaultPosition, wxDefaultSize, 0 );
	m_FindInfo->Wrap( -1 );
	m_FindInfo->SetFont(font);
	toolbar->AddControl( m_FindInfo );

	m_FindText = new wxTextCtrl( toolbar, ID_SEQUENCE_TXT, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 200,-1 ) );
	m_FindText->SetFont(font);
	toolbar->AddControl( m_FindText );

	toolbar->AddTool(ID_SEQUENCE_BTN, _("Find"), wxArtProvider::GetBitmap(wxART_FIND), _("Find series by name"));
	toolbar->AddTool(wxID_SAVE, _("Export"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Export to external device"));
	toolbar->Realize();
	return toolbar;
}

void FbFrameSequen::CreateMasterThread()
{
	wxDELETE(m_MasterLocker);
	m_MasterLocker = new FbMutexLocker;
	if (m_MasterThread) m_MasterThread->Wait();
	wxDELETE(m_MasterThread);
	m_MasterThread = new FbSeqnListThread(this, *m_MasterLocker, m_info, m_MasterList->GetSortedColumn());
	m_MasterThread->Execute();
}

void FbFrameSequen::OnMasterSelected(wxTreeEvent & event)
{
	FbSeqnListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbSeqnListData);
	if (data) FbMasterSeqname(data->GetCode()).Show(this);
}

void FbFrameSequen::FindSequence(const wxString &text)
{
	m_info = text;
	CreateMasterThread();
}

void FbFrameSequen::OpenSequence(const int sequence, const int book)
{
/*
	m_SequenceText = wxEmptyString;
	m_SequenceCode = sequence;
	(new MasterThread(m_MasterList, m_SequenceCode, m_MasterList->GetSortedColumn()))->Execute();
*/
}

void FbFrameSequen::UpdateBooklist()
{
	m_BooksPanel->EmptyBooks();
	FbSeqnListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbSeqnListData);
	if (data) FbMasterSeqname(data->GetCode()).Show(this);
}

void FbFrameSequen::OnColClick(wxListEvent& event)
{
	CreateMasterThread();
}

void FbFrameSequen::OnBooksCount(wxCommandEvent& event)
{
/*
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) m_MasterList->SetItemText(item, 1, wxString::Format(wxT("%d "), GetBookCount()));
	event.Skip();
*/
}

void FbFrameSequen::OnFindEnter(wxCommandEvent& event)
{
	FindSequence(m_FindText->GetValue());
}

void FbFrameSequen::ShowFullScreen(bool show)
{
	m_FindText->Show(!show);
	m_FindInfo->Show(!show);
	if (m_ToolBar) m_ToolBar->Show(!show);
	Layout();
}

FbFrameSequen::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_DELETE,  _("Delete"));
}

void FbFrameSequen::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameSequen::ShowContextMenu(const wxPoint& pos, wxTreeItemId item)
{
	FbSeqnListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbSeqnListData);
	if (data) FbMasterSeqname(data->GetCode()).Show(this);
	int id = data ? data->GetCode() : 0;
	MasterMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameSequen::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbSequenDlg::Append(newname);
	if (id == 0) return;

	FbCacheData * cache = new FbCacheData(id, newname);
	FbCollection::AddSeqn(cache);

	m_MasterList->Append(new FbSeqnListData(id));
}

void FbFrameSequen::OnMasterModify(wxCommandEvent& event)
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model == NULL) return;

	FbSeqnListData * current = wxDynamicCast(m_MasterList->GetCurrent(), FbSeqnListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbSequenDlg::Modify(old_id, newname);
	if (new_id == 0) return;

	FbCacheData * cache = new FbCacheData(new_id, newname);
	FbCollection::AddSeqn(cache);

	if (new_id != old_id) model->Delete(new_id);
	m_MasterList->Replace(new FbSeqnListData(new_id));
}

void FbFrameSequen::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbSeqnListData * current = wxDynamicCast(model->GetCurrent(), FbSeqnListData);
	if (current == NULL) return;

	int id = current->GetCode();

	wxString msg = wxString::Format(_("Delete series «%s»?"), current->GetValue(*model).c_str());
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		wxString sql1 = wxString::Format(wxT("DELETE FROM sequences WHERE id=%d"), id);
		wxString sql2 = wxString::Format(wxT("DELETE FROM bookseq WHERE id_seq=%d"), id);
		(new FbUpdateThread(sql1, sql2))->Execute();
		m_MasterList->Delete();
	}
}

FbFrameSequen::MenuBar::MenuBar()
{
	Append(new MenuFile,   _("&File"));
	Append(new MenuLib,    _("&Library"));
	Append(new MenuFrame,  _("&Catalog"));
	Append(new MenuMaster, _("&Series"));
	Append(new MenuBook,   _("&Books"));
	Append(new MenuView,   _("&View"));
	Append(new MenuSetup,  _("&Tools"));
	Append(new MenuWindow, _("&Window"));
	Append(new MenuHelp,   _("&?"));
}

FbFrameSequen::MenuMaster::MenuMaster()
{
	Append(ID_MASTER_APPEND,  _("Append"));
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_DELETE,  _("Delete"));
}

wxMenuBar * FbFrameSequen::CreateMenuBar()
{
	return new MenuBar;
}

void FbFrameSequen::OnAppendSequence(wxCommandEvent& event)
{
	wxString title = event.GetString();
	if (title.IsEmpty()) title = _("(Misc.)");
	m_BooksPanel->AppendAuthor( event.GetInt(), title );
}

void FbFrameSequen::OnAppendAuthor(wxCommandEvent& event)
{
	wxString title = event.GetString();
	if (event.GetInt() == 0) title = wxGetTranslation(strNobody);
	m_BooksPanel->AppendSequence( event.GetInt(), title, new FbAuthorData(event.GetInt()) );
}

void FbFrameSequen::OnModel( FbArrayEvent& event )
{
	FbSeqnListModel * model = new FbSeqnListModel(event.GetArray());
	m_MasterList->AssignModel(model);
}

void FbFrameSequen::OnArray( FbArrayEvent& event )
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}
