#include "FbFilterDlg.h"
#include "models/FbFilterTree.h"
#include "FbFilterThread.h"
#include "FbProgressDlg.h"
#include <wx/imaglist.h>

BEGIN_EVENT_TABLE( FbFilterDlg, wxDialog )
	EVT_BUTTON( wxID_NO, FbFilterDlg::OnNoButton )
	EVT_FB_MODEL(ID_TREE_LANG, FbFilterDlg::OnTreeModel)
	EVT_FB_MODEL(ID_TREE_TYPE, FbFilterDlg::OnTreeModel)
END_EVENT_TABLE()

FbFilterDlg::FbFilterDlg(FbFilterObj & filter)
	: FbDialog( NULL, wxID_ANY, _("Filter settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
		m_filter(filter), m_thread(NULL)
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
	m_treeLang->AddColumn(0, _("Language"), -10);
	bSizerList->Add( m_treeLang, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_treeType = new FbTreeViewCtrl(this, ID_TREE_TYPE, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_CHECKBOX);
	m_treeType->AddColumn(0, _("File extension"), -10);
	bSizerList->Add( m_treeType, 1, wxEXPAND|wxALL, 5 );

	bSizerMain->Add( bSizerList, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizer = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizer, 0, wxEXPAND|wxALL, 5 );

	int last = FbParams(DB_LAST_BOOK);
	int next = FbParams(DB_NEW_BOOK) + 1;
	if ( last != next ) {
		m_thread = new FbFilterTreeThread(this, next);
		m_thread->Execute();
	} else {
		FbFilterTreeModel * model;
		model = new FbFilterTreeModel(FbParams(DB_LANG_LIST), filter.m_lang);
		m_treeLang->AssignModel(model);
		model = new FbFilterTreeModel(FbParams(DB_TYPE_LIST), filter.m_type);
		m_treeType->AssignModel(model);
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

	FbFilterTreeModel * model;

	model = wxDynamicCast(m_treeLang->GetModel(), FbFilterTreeModel);
	if (model) filter.m_lang = model->GetSel();

	model = wxDynamicCast(m_treeType->GetModel(), FbFilterTreeModel);
	if (model) filter.m_type = model->GetSel();
}

void FbFilterDlg::OnTreeModel( FbModelEvent& event )
{
	FbFilterTreeModel * model = wxDynamicCast(event.GetModel(), FbFilterTreeModel);
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(event.GetId()), FbTreeViewCtrl);
	if (model && treeview) {
		switch (event.GetId()) {
			case ID_TREE_LANG: model->SetSel(m_filter.m_lang); break;
			case ID_TREE_TYPE: model->SetSel(m_filter.m_type); break;
		}
		treeview->AssignModel(event.GetModel());
	} else {
		delete event.GetModel();
	}
}

bool FbFilterDlg::Execute(FbFilterObj & filter)
{
	FbFilterDlg dlg(filter);
	int res = dlg.ShowModal() == wxID_OK;
	if (res) {
		dlg.Assign(filter);
		filter.m_enabled = true;
		filter.Save();
/*
		FbProgressDlg progress(NULL);
		FbThread * thread = new FbFilterThread(&progress, filter);
		progress.RunThread(thread);
		return (progress.ShowModal() == wxID_OK);
*/		
	}
	return res;
}

