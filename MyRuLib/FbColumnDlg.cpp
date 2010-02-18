#include "FbColumnDlg.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbBookEvent.h"

///////////////////////////////////////////////////////////////////////////

FbColumnDlg::FbColumnDlg( wxWindow* parent, wxWindowID id)
	: FbDialog( parent, id, _("Колонки"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxCheckBox * checkbox;

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	checkbox = new wxCheckBox( this, ID_COLUMN_GENRE, _("Genre"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_COLUMN_RATING, _("Rating"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_COLUMN_LANG, _("Language"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_COLUMN_TYPE, _("Extension"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_COLUMN_SYZE, _("Size, Kb"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	this->SetSize(GetBestSize());
}

void FbColumnDlg::Assign(bool write)
{
	struct Struct{
		int param;
		ID control;
	};

	const Struct ids[] = {
		{FB_COLUMN_LANG, ID_COLUMN_LANG},
		{FB_COLUMN_TYPE, ID_COLUMN_TYPE},
		{FB_COLUMN_SYZE, ID_COLUMN_SYZE},
		{FB_COLUMN_GENRE, ID_COLUMN_GENRE},
		{FB_COLUMN_RATING, ID_COLUMN_RATING},
	};

	const size_t idsCount = sizeof(ids) / sizeof(Struct);

	FbParams params;

	for (size_t i=0; i<idsCount; i++) {
		if (wxCheckBox * control = (wxCheckBox*)FindWindowById(ids[i].control)) {
			if (write)
				params.SetValue(ids[i].param, control->GetValue());
			else
				control->SetValue(params.GetValue(ids[i].param) != 0);
		}
	}

	if (write) FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_UPDATE_FONTS).Post();
}

void FbColumnDlg::Execute(wxWindow* parent)
{
	FbColumnDlg dlg(parent);

	dlg.Assign(false);

	if (dlg.ShowModal() == wxID_OK) dlg.Assign(true);
};

