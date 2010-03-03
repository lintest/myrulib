#include "FbFrameDate.h"
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

BEGIN_EVENT_TABLE(FbFrameDate, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameDate::OnMasterSelected)
	EVT_COMMAND(ID_BOOKS_COUNT, fbEVT_BOOK_ACTION, FbFrameDate::OnBooksCount)
END_EVENT_TABLE()

FbFrameDate::FbFrameDate(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_DATE, GetTitle()), m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	CreateControls();
}

void FbFrameDate::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	m_ToolBar = CreateToolBar();
	sizer->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new MasterList(splitter, ID_MASTER_LIST, wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER);
	m_MasterList->AddColumn(_("Date"), 40, wxALIGN_LEFT);
	m_MasterList->SetFocus();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	(new MasterThread(m_MasterList))->Execute();
}

void FbFrameDate::Localize(bool bUpdateMenu)
{
	FbFrameBase::Localize(bUpdateMenu);
	m_MasterList->SetColumnText(0, _("Date"));
}

void FbFrameDate::SelectFirstAuthor(const int book)
{
	m_BooksPanel->EmptyBooks(book);

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_MasterList->GetFirstChild(m_MasterList->GetRootItem(), cookie);
	if (item.IsOk()) m_MasterList->SelectItem(item);
}

void FbFrameDate::OnMasterSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbMasterData * data = m_MasterList->GetItemData(selected);
		if (data) data->Show(this);
	}
}

void FbFrameDate::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameDate::UpdateBooklist()
{
	m_BooksPanel->EmptyBooks();
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (data) data->Show(this);
}

void FbFrameDate::OnBooksCount(wxCommandEvent& event)
{
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) m_MasterList->SetItemText(item, 1, wxString::Format(wxT("%d"), GetBookCount()));
	event.Skip();
}

wxCriticalSection FbFrameDate::MasterThread::sm_queue;

void * FbFrameDate::MasterThread::Entry()
{
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_EMPTY_BOOKS).Post(m_frame);

	wxCriticalSectionLocker locker(sm_queue);

	try {
		FbCommonDatabase database;
		wxString sql = wxT("SELECT DISTINCT created FROM books ORDER BY created DESC");
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		FbMasterEvent(ID_EMPTY_MASTERS).Post(m_frame);
		while (result.NextRow()) {
		    int id = result.GetInt(0);
			FbMasterDate * data = new FbMasterDate(id);
			FbMasterEvent(ID_APPEND_MASTER, data->GetDate().FormatDate(), data, id).Post(m_frame);
		}
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

BEGIN_EVENT_TABLE(FbFrameDate::MasterList, FbMasterList)
	EVT_FB_MASTER(ID_APPEND_MASTER, FbFrameDate::MasterList::OnAppendMaster)
END_EVENT_TABLE()

void FbFrameDate::MasterList::OnAppendMaster(FbMasterEvent& event)
{
	FbTreeListUpdater updater(this);

    int id = event.m_number;
    wxDateTime date = ((FbMasterDate*)event.m_data)->GetDate();

	if (m_parent.IsOk() && m_month == id / 100) {
	    //nothing to do
	} else if (m_owner.IsOk() && m_year == id / 10000) {
        m_month = id / 100;
        m_parent = AppendItem(m_owner, date.Format(wxT("%B %Y")));
        SetItemBold(m_parent, true);
	} else {
        m_year = id / 10000;
        m_month = id / 100;
        m_owner = AppendItem(GetRootItem(), date.Format(wxT("%Y")));
        m_parent = AppendItem(m_owner, date.Format(wxT("%B %Y")));
        SetItemBold(m_owner, true);
        SetItemBold(m_parent, true);
        Expand(m_owner);
    }

	wxTreeItemId item = AppendItem(m_parent, event.GetString(), -1, -1, event.m_data);
    Expand(m_parent);
}

