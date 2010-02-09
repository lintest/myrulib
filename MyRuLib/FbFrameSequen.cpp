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

BEGIN_EVENT_TABLE(FbFrameSequen, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameSequen::OnAuthorSelected)
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
END_EVENT_TABLE()

FbFrameSequen::FbFrameSequen(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_SEQUEN, _("Серии")), m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	CreateControls();
}

void FbFrameSequen::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	wxBoxSizer* bSizerSeq;
	bSizerSeq = new wxBoxSizer( wxHORIZONTAL );

	m_FindInfo = new wxStaticText( this, wxID_ANY, _("Серия:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_FindInfo->Wrap( -1 );
	bSizerSeq->Add( m_FindInfo, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );

	m_FindText = new wxTextCtrl( this, ID_SEQUENCE_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 200,-1 ) );
	bSizerSeq->Add( m_FindText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	m_ToolBar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizerSeq->Add( m_ToolBar, 1, wxALIGN_CENTER_VERTICAL);

	sizer->Add(bSizerSeq, 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbMasterList(splitter, ID_MASTER_LIST, wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER);
	m_MasterList->AddColumn(_("Серия"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(_("Кол."), 10, wxALIGN_RIGHT);
	m_MasterList->SetFocus();
	m_MasterList->SetSortedColumn(1);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	FindSequence(wxEmptyString);
}

wxToolBar * FbFrameSequen::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(FbParams::GetFont(FB_FONT_TOOL));
	toolbar->AddTool(ID_SEQUENCE_BTN, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Найти серию по наименованию"));
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();
	return toolbar;
}

void FbFrameSequen::SelectFirstAuthor(const int book)
{
	m_BooksPanel->EmptyBooks(book);

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_MasterList->GetFirstChild(m_MasterList->GetRootItem(), cookie);
	if (item.IsOk()) m_MasterList->SelectItem(item);
}

void FbFrameSequen::OnAuthorSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbMasterData * data = m_MasterList->GetItemData(selected);
		if (data) data->Show(this);
	}
}

void FbFrameSequen::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameSequen::FindSequence(const wxString &text)
{
	m_SequenceText = text;
	m_SequenceCode = 0;
	(new MasterThread(m_MasterList, m_SequenceText, m_MasterList->GetSortedColumn()))->Execute();
}

void FbFrameSequen::OpenSequence(const int sequence, const int book)
{
	m_SequenceText = wxEmptyString;
	m_SequenceCode = sequence;
	(new MasterThread(m_MasterList, m_SequenceCode, m_MasterList->GetSortedColumn()))->Execute();
}

void FbFrameSequen::UpdateBooklist()
{
	m_BooksPanel->EmptyBooks();
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (data) data->Show(this);
}

void FbFrameSequen::OnColClick(wxListEvent& event)
{
	if (m_SequenceCode)
		(new MasterThread(m_MasterList, m_SequenceCode, m_MasterList->GetSortedColumn()))->Execute();
	else
		(new MasterThread(m_MasterList, m_SequenceText, m_MasterList->GetSortedColumn()))->Execute();
}

void FbFrameSequen::OnBooksCount(wxCommandEvent& event)
{
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) m_MasterList->SetItemText(item, 1, wxString::Format(wxT("%d"), GetBookCount()));
	event.Skip();
}

wxCriticalSection FbFrameSequen::MasterThread::sm_queue;

void * FbFrameSequen::MasterThread::Entry()
{
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_EMPTY_BOOKS).Post(m_frame);

	wxCriticalSectionLocker locker(sm_queue);

	try {
		bool bBindText = false;
		FbCommonDatabase database;
		FbSearchFunction search(m_text);
		wxString sql = wxT("SELECT id, value as name, number FROM sequences ");
		if (m_code) {
			sql += wxString::Format(wxT("WHERE id=%d"), m_code);
		} else if ( m_text.IsEmpty() ) {
			// Nothing to do !
		} else if ( FbSearchFunction::IsFullText(m_text) && database.TableExists(wxT("fts_seqn")) ) {
			sql += wxT("WHERE id IN (SELECT docid FROM fts_seqn WHERE fts_seqn MATCH ?)");
			bBindText = true;
		} else {
			sql += wxT("WHERE SEARCH(value)");
			database.CreateFunction(wxT("SEARCH"), 1, search);
		}
		sql += wxT(" ORDER BY ") + GetOrder() + FbParams::GetLimit();
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		if (bBindText) stmt.Bind(1, FbSearchFunction::AddAsterisk(m_text));
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		FbMasterEvent(ID_EMPTY_MASTERS).Post(m_frame);
		while (result.NextRow()) {
			FbMasterSeqname * data = new FbMasterSeqname( result.GetInt(0) );
			FbMasterEvent(ID_APPEND_MASTER, result.GetString(1), data, result.GetInt(2)).Post(m_frame);
		}
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

wxString FbFrameSequen::MasterThread::GetOrder()
{
	switch (m_order) {
		case -2: return wxT("number desc, name desc");
		case -1: return wxT("name desc");
		case  2: return wxT("number, name");
		default: return wxT("name ");
	}
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
	Append(ID_MASTER_APPEND, _("Добавить"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY, _("Изменить"));
	Append(ID_MASTER_DELETE, _("Удалить"));
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
	int id = 0;
	if (item.IsOk()) {
		FbMasterData * data = m_MasterList->GetItemData(item);
		if (data) id = data->GetId();
	}
	MasterMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

FbFrameSequen::EditDlg::EditDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ), m_id(id)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_text.Create( this, wxID_ANY, wxT("Наименование серии:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( &m_text, 0, wxEXPAND|wxALL, 5 );

	m_edit.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_edit.SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( &m_edit, 0, wxEXPAND|wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	wxSize newSize = GetBestSize();
	this->SetSize(newSize);

	m_edit.SetFocus();
}

int FbFrameSequen::EditDlg::Append(wxString &newname)
{
	EditDlg dlg(_("Добавить серию"));
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetValue();
	return ok ? dlg.DoAppend() : 0;
}

int FbFrameSequen::EditDlg::Modify(int id, wxString &newname)
{
	EditDlg dlg(_("Изменить серию"), id);
	bool ok = dlg.Load(id) && dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetValue();
	return ok ? dlg.DoUpdate() : 0;
}

bool FbFrameSequen::EditDlg::Load(int id)
{
	wxString sql = wxT("SELECT value FROM sequences WHERE id=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	bool ok = result.NextRow();
	if (ok) m_edit.SetValue(result.GetString(0));
	return ok;
}

int FbFrameSequen::EditDlg::Find()
{
	wxString sql = wxT("SELECT id FROM sequences WHERE value=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, m_edit.GetValue());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0) : 0;
}

int FbFrameSequen::EditDlg::DoAppend()
{
	wxString sql = wxT("INSERT INTO sequences(value, id) VALUES (?,?)");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	m_id = - m_database.NewId(DB_NEW_SEQUENCE);
	stmt.Bind(1, GetValue());
	stmt.Bind(2, m_id);
	stmt.ExecuteUpdate();
	return m_id;
}

int FbFrameSequen::EditDlg::DoModify()
{
	wxString sql = wxT("UPDATE sequences SET value=? WHERE id=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, GetValue());
	stmt.Bind(2, m_id);
	stmt.ExecuteUpdate();
	return m_id;
}

int FbFrameSequen::EditDlg::DoReplace()
{
	wxString sql = wxT("UPDATE bookseq SET id_seq=? WHERE id_seq=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, m_exists);
	stmt.Bind(2, m_id);
	stmt.ExecuteUpdate();
	return m_exists;
}

int FbFrameSequen::EditDlg::DoUpdate()
{
	return m_exists ? DoReplace() : DoModify();
}

void FbFrameSequen::EditDlg::EndModal(int retCode)
{
	if ( retCode == wxID_OK) {
		if (GetValue().IsEmpty()) {
			wxMessageBox(_("Не заполнено наименование серии."), GetTitle());
			return;
		}
		m_exists = Find();
		if (m_exists) {
			wxString msg = _("Такая серия уже существует.");
			wxString title = GetTitle() + wxT("…");
			if (m_id) {
				msg += _("\nОбъединить две серии?");
				bool ok = wxMessageBox(msg, title, wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
				if (!ok) return;
			} else {
				wxMessageBox(msg, title, wxICON_EXCLAMATION);
				return;
			}
		}
	}
	FbDialog::EndModal(retCode);
}
void FbFrameSequen::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = EditDlg::Append(newname);
	if (id) {
		FbMasterData * data = new FbMasterSeqname(id);
		wxTreeItemId item = m_MasterList->InsertItem(m_MasterList->GetRootItem(), m_MasterList->GetSelection(), newname, -1, -1, data);
		m_MasterList->SelectItem(item);
	}
}

void FbFrameSequen::OnMasterModify(wxCommandEvent& event)
{
    wxTreeItemId selected = m_MasterList->GetSelection();
	FbMasterData * data = selected.IsOk() ? m_MasterList->GetItemData(selected) : NULL;
	if (data && data->GetId()) {
		wxString newname;
		int old_id = data->GetId();
		int new_id = EditDlg::Modify(old_id, newname);
		if (new_id) ReplaceData(old_id, new_id, selected, newname);
	}
}

void FbFrameSequen::ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname)
{
	if (selected.IsOk()) m_MasterList->SetItemText(selected, newname);
	if (old_id != new_id) {
		FbMasterSeqname deleted(new_id);
		m_MasterList->DeleteItem(deleted);
		delete m_MasterList->GetItemData(selected);
		FbMasterData * data = new FbMasterSeqname(new_id);
		m_MasterList->SetItemData(selected, data);
		m_MasterList->SelectItem(selected);
	}
}

void FbFrameSequen::OnMasterDelete(wxCommandEvent& event)
{
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (!data) return;
	int id = data->GetId();
	if (!id) return;

	wxTreeItemId selected = m_MasterList->GetSelection();
	wxString name = m_MasterList->GetItemText(selected);
	wxString msg = wxString::Format(_("Удалить серию «%s»?"), name.c_str());
	bool ok = wxMessageBox(msg, _("Удаление"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		wxString sql1 = wxString::Format(wxT("DELETE FROM sequences WHERE id=%d"), id);
		wxString sql2 = wxString::Format(wxT("DELETE FROM bookseq WHERE id_seq=%d"), id);
		(new FbUpdateThread(sql1, sql2))->Execute();
		m_MasterList->Delete(selected);
	}
}

FbFrameSequen::MenuBar::MenuBar()
{
	Append(new MenuFile,   _("Файл"));
	Append(new MenuLib,    _("Библиотека"));
	Append(new MenuFrame,  _("Картотека"));
	Append(new MenuMaster, _("Серии"));
	Append(new MenuBook,   _("Книги"));
	Append(new MenuView,   _("Вид"));
	Append(new MenuSetup,  _("Сервис"));
	Append(new MenuWindow, _("Окно"));
	Append(new MenuHelp,   _("?"));
}

FbFrameSequen::MenuMaster::MenuMaster()
{
	Append(ID_MASTER_APPEND, _("Добавить"));
	Append(ID_MASTER_MODIFY, _("Изменить"));
	Append(ID_MASTER_DELETE, _("Удалить"));
}

wxMenuBar * FbFrameSequen::CreateMenuBar()
{
	return new MenuBar;
}

void FbFrameSequen::OnAppendSequence(wxCommandEvent& event)
{
	wxString title = event.GetString();
	if (title.IsEmpty()) title = strOtherSequence;
	m_BooksPanel->AppendAuthor( event.GetInt(), title );
}

void FbFrameSequen::OnAppendAuthor(wxCommandEvent& event)
{
	m_BooksPanel->AppendSequence( event.GetInt(), event.GetString(), new FbAuthorData(event.GetInt()));
}

