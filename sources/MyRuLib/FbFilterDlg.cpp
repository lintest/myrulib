#include "FbFilterDlg.h"
#include "FbFilterTree.h"
#include <wx/imaglist.h>

/*
FbFilterList::FbFilterList(wxWindow *parent, wxWindowID id, const wxString &title)
	:FbCheckList(parent, id, wxTR_FULL_ROW_HIGHLIGHT | wxTR_MULTIPLE | wxSUNKEN_BORDER)
{
	SetFont( FbParams::GetFont(FB_FONT_DLG) );
	AddColumn (title, 10, wxALIGN_LEFT);
	SetMinSize( wxSize(100, 100) );
	SetItemBold( AddRoot(_("All"), 0), true );
}

int FbFilterList::Append(wxTreeItemId parent, wxString &text, const wxString & filter)
{
	text.Replace(wxT(","), wxEmptyString, true);
	text.Replace(wxT("'"), wxEmptyString, true);
	text.Replace(wxT("\""), wxEmptyString, true);
	if (text.IsEmpty()) return -1;
	int image = filter.IsEmpty() ? 1 : filter.Find(wxT("'") + text + wxT("'")) == wxNOT_FOUND ? 0 : 1;
	AppendItem(parent, text, image);
	return image;
}

wxString FbFilterList::Load(FbDatabase & database, const wxString & sql, const wxString & filter)
{
	wxString list;
	wxTreeItemId root = GetRootItem();
	Freeze();

	int all = -1;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while ( result.NextRow() ) {
		wxString text = result.GetString(0);
		int image = Append(root, text, filter);
		if (image<0) continue;
		if (!list.IsEmpty()) list += wxT(",");
		list += text;
		if (all < 0) all = image; else if (all != image) all = 2;
	}
	if (all > 0) SetItemImage(root, all);

	Expand(root);
	Thaw();
	return list;
}

void FbFilterList::Read(const wxString & filter, int key)
{
	wxTreeItemId root = GetRootItem();
	Freeze();
	wxString list = FbParams::GetText(key);
	int all = -1;

	int pos;
	do {
		pos = list.Find(wxT(','));
		wxString text = pos == wxNOT_FOUND ? list : list.Left(pos);
		list = list.Mid(pos + 1);
		int image = Append(root, text, filter);
		if (all < 0) all = image; else if (all != image && image>=0) all = 2;
	} while (pos != wxNOT_FOUND);

	if (all > 0) SetItemImage(root, all);
	Expand(root);
	Thaw();
}

wxString FbFilterList::GetValue()
{
	wxString result;
	bool all = true;
	wxTreeItemIdValue cookie;
	wxTreeItemId parent = GetRootItem();
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		if (GetItemImage(child) == 1) {
			if (!result.IsEmpty()) result += wxT(",");
			result += wxT("'") + GetItemText(child) + wxT("'");
		} else all = false;
		child = GetNextChild(parent, cookie);
	}
	return all ? wxString(wxEmptyString) : result;
}

///////////////////////////////////////////////////////////////////////////

*/

BEGIN_EVENT_TABLE( FbFilterDlg, wxDialog )
	EVT_BUTTON( wxID_NO, FbFilterDlg::OnNoButton )
	EVT_FB_MODEL(ID_TREE_LANG, FbFilterDlg::OnTreeModel)
	EVT_FB_MODEL(ID_TREE_TYPE, FbFilterDlg::OnTreeModel)
END_EVENT_TABLE()

FbFilterDlg::FbFilterDlg(FbFilterObj & filter)
	: FbDialog( NULL, wxID_ANY, _("Filter settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
		m_thread(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerOwner;
	sbSizerOwner = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Show books by allocation") ), wxHORIZONTAL );

	m_checkLib = new wxCheckBox( this, ID_CHECK_LIB, _("Library books (web)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkLib->SetValue( filter.m_lib );
	sbSizerOwner->Add( m_checkLib, 0, wxALL|wxEXPAND, 5 );

	m_checkUsr = new wxCheckBox( this, ID_CHECK_USR, _("Local books"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkUsr->SetValue( filter.m_usr );
	sbSizerOwner->Add( m_checkUsr, 0, wxALL|wxEXPAND, 5 );

	bSizerMain->Add( sbSizerOwner, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizerList;
	bSizerList = new wxBoxSizer( wxHORIZONTAL );

	m_treeLang = new FbTreeViewCtrl(this, ID_TREE_LANG, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_CHECKBOX);
	m_treeLang->AddColumn(0, _("Language"), 1);
	bSizerList->Add( m_treeLang, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_treeType = new FbTreeViewCtrl(this, ID_TREE_TYPE, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_CHECKBOX);
	m_treeType->AddColumn(0, _("File extension"), 1);
	bSizerList->Add( m_treeType, 1, wxEXPAND|wxALL, 5 );

	bSizerMain->Add( bSizerList, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizer = CreateStdDialogButtonSizer( wxYES | wxNO | wxCANCEL );
	bSizerMain->Add( sdbSizer, 0, wxEXPAND|wxALL, 5 );

	int last = FbParams::GetValue(DB_LAST_BOOK);
	int next = FbParams::GetValue(DB_NEW_BOOK) + 1;
	if ( last != next ) {
		m_thread = new FbFilterTreeThread(this, last);
		m_thread->Execute();
	} else {
		m_treeLang->AssignModel(new FbFilterTreeModel(FbParams::GetText(DB_LANG_LIST)));
		m_treeType->AssignModel(new FbFilterTreeModel(FbParams::GetText(DB_TYPE_LIST)));
	}

	this->SetSizer( bSizerMain );
	this->Layout();
}

FbFilterDlg::~FbFilterDlg()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
}

void FbFilterDlg::OnNoButton( wxCommandEvent& event )
{
	wxDialog::EndModal( wxID_NO );
}

void FbFilterDlg::Assign(FbFilterObj & filter)
{
	filter.m_lib = m_checkLib->GetValue();
	filter.m_usr = m_checkUsr->GetValue();
//	filter.m_lang = m_treeLang->GetValue();
//	filter.m_type = m_treeType->GetValue();
}

void FbFilterDlg::OnTreeModel( FbModelEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(event.GetId()), FbTreeViewCtrl);
	if (treeview) {
		treeview->AssignModel(event.GetModel());
	} else {
		delete event.GetModel();
	}
}

bool FbFilterDlg::Execute(FbFilterObj & filter)
{
	FbFilterDlg dlg(filter);
	int res = dlg.ShowModal();

	switch ( res ) {
		case wxID_YES: {
			dlg.Assign(filter);
			filter.m_enabled = true;
			filter.Save();
		} break;
		case wxID_NO: {
			filter.m_enabled = false;
			FbParams().SetValue(FB_USE_FILTER, 0);
		} break;
	}

	return  res != wxID_CANCEL;
}

