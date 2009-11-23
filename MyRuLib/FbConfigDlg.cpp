///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/artprov.h>
#include <wx/statline.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textdlg.h>
#include "FbConfigDlg.h"
#include "FbParams.h"
#include "ZipReader.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FbConfigDlg, wxDialog )
	EVT_BUTTON( ID_LIBRARY_DIR_BTN, FbConfigDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_WANRAIK_DIR_BTN, FbConfigDlg::OnSelectFolderClick )
END_EVENT_TABLE()

FbConfigDlg::FbConfigDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style )
{
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textCtrl1;
	wxStaticText* m_staticText2;
	wxTextCtrl* m_textCtrl2;
	wxBitmapButton* m_bpButton2;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_textCtrl4;
	wxStdDialogButtonSizer* m_sdbSizerBtn;
	wxButton* m_sdbSizerBtnOK;
	wxButton* m_sdbSizerBtnCancel;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Название библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizerMain->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_textCtrl1 = new wxTextCtrl( this, ID_LIBRARY_TITLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetMinSize( wxSize( 200,-1 ) );

	bSizerMain->Add( m_textCtrl1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Размещение файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizerMain->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl2 = new wxTextCtrl( this, ID_LIBRARY_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl2->SetMinSize( wxSize( 200,-1 ) );

	bSizer2->Add( m_textCtrl2, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_bpButton2 = new wxBitmapButton( this, ID_LIBRARY_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_bpButton2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizerMain->Add( bSizer2, 0, wxEXPAND, 5 );

	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Краткое описание:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizerMain->Add( m_staticText4, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_textCtrl4 = new wxTextCtrl( this, ID_LIBRARY_DESCR, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textCtrl4->SetMinSize( wxSize( 200, 60 ) );

	bSizerMain->Add( m_textCtrl4, 1, wxALL|wxEXPAND, 5 );

	m_sdbSizerBtn = new wxStdDialogButtonSizer();
	m_sdbSizerBtnOK = new wxButton( this, wxID_OK );
	m_sdbSizerBtnOK->SetDefault();
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnOK );
	m_sdbSizerBtnCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnCancel );
	m_sdbSizerBtn->Realize();
	bSizerMain->Add( m_sdbSizerBtn, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	SetAffirmativeId(wxID_OK);
	SetEscapeId(wxID_CANCEL);
}

void FbConfigDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxTextCtrl * textCtrl = (wxTextCtrl*)FindWindowById( event.GetId() - 1);

	if (!textCtrl) return;

	wxDirDialog dlg(
		this,
		_("Выберите директорию"),
		textCtrl->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK)  textCtrl->SetValue(dlg.GetPath());
}

void FbConfigDlg::Assign(bool write)
{
	enum Type {
		tText,
		tCheck,
		tRadio,
	};
	struct Struct{
		int param;
		ID control;
		Type type;
	};

	const Struct ids[] = {
		{DB_LIBRARY_TITLE, FbConfigDlg::ID_LIBRARY_TITLE, tText},
		{DB_LIBRARY_DIR,   FbConfigDlg::ID_LIBRARY_DIR_TXT, tText},
		{DB_WANRAIK_DIR,   FbConfigDlg::ID_WANRAIK_DIR_TXT, tText},
		{DB_LIBRARY_DESCR, FbConfigDlg::ID_LIBRARY_DESCR, tText},
	};

	const size_t idsCount = sizeof(ids) / sizeof(Struct);

	FbParams params;

	for (size_t i=0; i<idsCount; i++) {
		switch (ids[i].type) {
			case tText:
				if (wxTextCtrl * control = (wxTextCtrl*)FindWindowById(ids[i].control)) {
					if (write)
						params.SetText(ids[i].param, control->GetValue());
					else
						control->SetValue(params.GetText(ids[i].param));
				} break;
			case tCheck:
				if (wxCheckBox * control = (wxCheckBox*)FindWindowById(ids[i].control)) {
					if (write)
						params.SetValue(ids[i].param, control->GetValue());
					else
						control->SetValue(params.GetValue(ids[i].param) != 0);
				} break;
			case tRadio:
				if (wxRadioBox * control = (wxRadioBox*)FindWindowById(ids[i].control)) {
					if (write)
						params.SetValue(ids[i].param, control->GetSelection());
					else
						control->SetSelection(params.GetValue(ids[i].param));
				} break;
		}

	}
}

void FbConfigDlg::Execute(wxWindow* parent)
{
	FbConfigDlg dlg(parent, wxID_ANY, _("Параметры библиотеки"), wxDefaultPosition, wxSize(400, 300));
	dlg.Assign(false);
	if (dlg.ShowModal() == wxID_OK) {
		dlg.Assign(true);
		ZipReader::Init();
	}
}

