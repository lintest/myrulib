#include "FbColumnDlg.h"
#include "FbColumns.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbBookEvent.h"
#include <wx/artprov.h>

//-----------------------------------------------------------------------------
//  FbBookListData
//-----------------------------------------------------------------------------

class FbFieldData: public FbModelData
{
	public:
		FbFieldData(int code, bool check)
			: m_code(code), m_name(FbColumns::GetName(code)), m_check(check) {}
		int GetCode() const
			{ return m_code; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return m_name; }
	protected:
		virtual void DoSetState(FbModel & model, int state)
			{ m_check = state == 1; }
		virtual int DoGetState(FbModel & model) const
			{ return m_check ? 1 : 0; }
	private:
		int m_code;
		wxString m_name;
		bool m_check;
		DECLARE_CLASS(FbFieldData);
};

IMPLEMENT_CLASS(FbFieldData, FbModelData)

//-----------------------------------------------------------------------------
//  FbColumnDlg
//-----------------------------------------------------------------------------

void FbColumnModel::MoveUp()
{
	size_t count = m_list.Count();
	if (1 < m_position && m_position <= count) {
		size_t index = m_position - 1;
		FbModelData * data = m_list.Detach(index);
		m_list.Insert(data, index - 1);
		m_position--;
	}
}

void FbColumnModel::MoveDown()
{
	if (!m_position) return;
	size_t count = m_list.Count();
	if (0 < m_position && m_position < count) {
		size_t index = m_position - 1;
		FbModelData * data = m_list.Detach(index);
		m_list.Insert(data, index + 1);
		m_position++;
	}
}

//-----------------------------------------------------------------------------
//  FbColumnDlg
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbColumnDlg, FbDialog)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbColumnDlg::OnItemSelected)
	EVT_MENU(wxID_UP, FbColumnDlg::OnMoveUp)
	EVT_MENU(wxID_DOWN, FbColumnDlg::OnMoveDown)
END_EVENT_TABLE()

FbColumnDlg::FbColumnDlg(wxWindow* parent, const wxArrayInt & columns)
	: FbDialog(parent, wxID_ANY, _("Columns"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_toolbar.Create(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT);
	m_toolbar.SetFont(FbParams(FB_FONT_TOOL));
	m_toolbar.AddTool(wxID_UP, _("Up"), wxART_GO_UP, _("Move field up"));
	m_toolbar.AddTool(wxID_DOWN, _("Down"), wxART_GO_DOWN, _("Move field down"));
	m_toolbar.Realize();
	bSizerMain->Add( &m_toolbar, 0, wxALL|wxEXPAND, 5 );

	m_fields.Create(this, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_VRULES | fbTR_CHECKBOX);
	m_fields.AddColumn(0, _("Column"), -10, wxALIGN_LEFT);
	m_fields.SetMinSize(wxSize(200, 200));
	m_fields.AssignModel(CreateModel(columns));
	bSizerMain->Add( &m_fields, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	SetSizer( bSizerMain );
	Layout();

	SetSize(GetBestSize());
}

FbModel * FbColumnDlg::CreateModel(const wxArrayInt & columns)
{
	m_model = new FbColumnModel;

	bool exists[BF_LAST];
	for (size_t i = 0; i < BF_LAST; i++) exists[i] = false;

	size_t count = columns.Count();
	for (size_t i = 0; i < count; i++) {
		int index = columns[i];
		if (BF_AUTH <= index && index < BF_LAST) {
			m_model->Append(new FbFieldData(index, true));
			exists[index] = true;
		}
	}

	for (size_t i = BF_AUTH; i < BF_LAST; i++) {
		if (exists[i]) continue;
		m_model->Append(new FbFieldData(i, false));
	}
	return m_model;
}

void FbColumnDlg::GetColumns(wxArrayInt & columns)
{
	columns.Empty();
	size_t count = m_model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = m_model->GetData(i);
		if (item.GetState() == 0) continue;
		FbFieldData * data = wxDynamicCast(&item, FbFieldData);
		if (data) columns.Add(data->GetCode());
	}
}

void FbColumnDlg::OnMoveUp(wxCommandEvent& event)
{
	m_model->MoveUp();
	m_fields.Refresh();
	DoSelected();
}

void FbColumnDlg::OnMoveDown(wxCommandEvent& event)
{
	m_model->MoveDown();
	m_fields.Refresh();
	DoSelected();
}

void FbColumnDlg::OnItemSelected(wxTreeEvent & event)
{
	DoSelected();
}

void FbColumnDlg::DoSelected()
{
	size_t count = m_model->GetRowCount();
	size_t pos = m_model->GetPosition();
	m_toolbar.EnableTool(wxID_UP, pos && pos > 1);
	m_toolbar.EnableTool(wxID_DOWN, pos && pos < count);
}

bool FbColumnDlg::Execute(wxWindow * parent, wxArrayInt & columns)
{
	FbColumnDlg dlg(parent, columns);
	bool result = dlg.ShowModal() == wxID_OK;
	if (result) dlg.GetColumns(columns);
	return result;
}

