#include "FbFilterDlg.h"
#include <wx/imaglist.h>
#include "FbBookList.h"
#include "FbParams.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FbFilterDlg, wxDialog )
	EVT_BUTTON( wxID_NO, FbFilterDlg::OnNoButton )
END_EVENT_TABLE()

FbFilterDlg::FbFilterDlg(FbFilterObj & filter)
	: FbDialog( NULL, wxID_ANY, _("Настройка фильтра"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerOwner;
	sbSizerOwner = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Отбор книг по принадлежности") ), wxHORIZONTAL );

	m_checkLib = new wxCheckBox( this, ID_CHECK_LIB, _("Библиотечные"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkLib->SetValue( filter.m_lib );
	sbSizerOwner->Add( m_checkLib, 0, wxALL|wxEXPAND, 5 );

	m_checkUsr = new wxCheckBox( this, ID_CHECK_USR, _("Собственные"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkUsr->SetValue( filter.m_usr );
	sbSizerOwner->Add( m_checkUsr, 0, wxALL|wxEXPAND, 5 );

	bSizerMain->Add( sbSizerOwner, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizerList;
	bSizerList = new wxBoxSizer( wxHORIZONTAL );

	m_treeLang = CreateTree(_("Язык"));;
	bSizerList->Add( m_treeLang, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_treeType = CreateTree(_("Тип файла"));;
	bSizerList->Add( m_treeType, 1, wxEXPAND|wxALL, 5 );

	bSizerMain->Add( bSizerList, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizer = CreateStdDialogButtonSizer( wxYES | wxNO | wxCANCEL );
	bSizerMain->Add( sdbSizer, 0, wxEXPAND|wxALL, 5 );

	wxString sql;
	FbCommonDatabase database;

	sql = wxT("SELECT DISTINCT lang, CASE WHEN lang='ru' THEN 1 ELSE 2 END AS number FROM books ORDER BY number, lang");
	FillTree(database, m_treeLang, sql, filter.m_lang);

	sql = wxT("SELECT distinct file_type, CASE WHEN file_type='fb2' THEN 1 ELSE 2 END AS number FROM books ORDER BY number, file_type");
	FillTree(database, m_treeType, sql, filter.m_type);

	this->SetSizer( bSizerMain );
	this->Layout();
}

FbTreeListCtrl * FbFilterDlg::CreateTree(const wxString & title)
{
	FbCheckList * treelist = new FbCheckList( this, ID_TREE_TYPE, wxTR_FULL_ROW_HIGHLIGHT | wxTR_MULTIPLE | wxSUNKEN_BORDER );
	treelist->SetFont( FbParams::GetFont(FB_FONT_MAIN) );
	treelist->AddColumn (title, 10, wxALIGN_LEFT);
	treelist->SetMinSize( wxSize( 100,100 ) );
	treelist->SetItemBold( treelist->AddRoot(wxT("Все"), 0), true );
	return treelist;
}

void FbFilterDlg::OnNoButton( wxCommandEvent& event )
{
	wxDialog::EndModal( wxID_NO );
}

wxString FbFilterDlg::GetText(FbTreeListCtrl* treelist)
{
	wxString result;
	bool all = true;
	wxTreeItemIdValue cookie;
	wxTreeItemId parent = treelist->GetRootItem();
	wxTreeItemId child = treelist->GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		if (treelist->GetItemImage(child) == 1) {
			if (!result.IsEmpty()) result += wxT(",");
			result += wxT("'") + treelist->GetItemText(child) + wxT("'");
		} else all = false;
		child = treelist->GetNextChild(parent, cookie);
	}
	return all ? wxString(wxEmptyString) : result;
}

void FbFilterDlg::Assign(FbFilterObj & filter)
{
	filter.m_lib = m_checkLib->GetValue();
	filter.m_usr = m_checkUsr->GetValue();
	filter.m_lang = GetText(m_treeLang);
	filter.m_type = GetText(m_treeType);
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

void FbFilterDlg::FillTree(FbDatabase & database, FbTreeListCtrl * treelist, const wxString & sql, const wxString & filter)
{
	wxTreeItemId root = treelist->GetRootItem();
	treelist->Freeze();
	try {
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		while ( result.NextRow() ) {
			wxString text = result.GetString(0);
			text.Replace(wxT(","), wxEmptyString, true);
			text.Replace(wxT("'"), wxEmptyString, true);
			text.Replace(wxT("\""), wxEmptyString, true);
			if (text.IsEmpty()) continue;
			int image = filter.Find(wxT("'") + text + wxT("'")) == wxNOT_FOUND ? 0 : 1;
			treelist->AppendItem(root, text, image);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	treelist->Expand(root);
	treelist->Thaw();
}

