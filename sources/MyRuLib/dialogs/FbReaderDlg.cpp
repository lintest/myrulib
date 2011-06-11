#include <wx/wx.h>
#include "FbReaderDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "controls/FbTreeView.h"
#include "FbViewerDlg.h"
#include "FbCollection.h"
#include "FbDataPath.h"
#include "controls/FbCustomCombo.h"
#include "FbLogoBitmap.h"
#include "MyRuLibApp.h"

#ifdef FB_INCLUDE_READER

//-----------------------------------------------------------------------------
//  FbReaderDlg::PanelMain
//-----------------------------------------------------------------------------

FbReaderDlg::PanelMain::PanelMain(wxWindow *parent)
	: wxPanel(parent)
{
	wxFlexGridSizer * fgSizerList = new wxFlexGridSizer(2, 0, 0 );
	fgSizerList->AddGrowableCol( 1 );
	fgSizerList->SetFlexibleDirection( wxBOTH );
	fgSizerList->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText * stTitle;
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Main font"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFontPickerCtrl * fpMainFont = new wxFontPickerCtrl( this, ID_FONT_DEFAULT, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE|wxFNTP_USE_TEXTCTRL  );
	fpMainFont->SetMaxPointSize( 100 );
	fgSizerList->Add( fpMainFont, 0, wxALL|wxEXPAND, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Font colour"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxColourPickerCtrl * cpFont = new wxColourPickerCtrl( this, ID_FONT_COLOUR);
	fgSizerList->Add( cpFont, 0, wxALL, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Background colour"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxColourPickerCtrl * cpBack = new wxColourPickerCtrl( this, ID_BACK_COLOUR);
	fgSizerList->Add( cpBack, 0, wxALL, 5 );
	
	this->SetSizer( fgSizerList );
	this->Layout();
}

//-----------------------------------------------------------------------------
//  FbReaderDlg::PanelPage
//-----------------------------------------------------------------------------

FbReaderDlg::PanelPage::PanelPage(wxWindow *parent)
	: wxPanel(parent)
{
	wxFlexGridSizer * fgSizerList = new wxFlexGridSizer(2, 0, 0 );
	fgSizerList->AddGrowableCol( 1 );
	fgSizerList->SetFlexibleDirection( wxBOTH );
	fgSizerList->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText * stTitle;
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Header font"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFontPickerCtrl * fpMainFont = new wxFontPickerCtrl( this, ID_FONT_HEADER, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE|wxFNTP_USE_TEXTCTRL  );
	fpMainFont->SetMaxPointSize( 100 );
	fgSizerList->Add( fpMainFont, 0, wxALL|wxEXPAND, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Header colour"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxColourPickerCtrl * cpFont = new wxColourPickerCtrl( this, ID_HEAD_COLOUR);
	fgSizerList->Add( cpFont, 0, wxALL, 5 );
	
	this->SetSizer( fgSizerList );
	this->Layout();
}

//-----------------------------------------------------------------------------
//  FbReaderDlg
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbReaderDlg, FbDialog )
END_EVENT_TABLE()

FbReaderDlg::FbReaderDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxNotebook * notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	notebook->AddPage( new PanelMain(notebook), _("General"), true );
	notebook->AddPage( new PanelPage(notebook), _("Page"), false );
	bSizerMain->Add( notebook, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

FbReaderDlg::~FbReaderDlg()
{
}

void FbReaderDlg::Assign(bool write)
{
	struct Struct {
		int param;
		ID control;
	};

	const Struct ids[] = {
		{FB_READER_FONT_DEFAULT, FbReaderDlg::ID_FONT_DEFAULT},
		{FB_READER_FONT_COLOUR,  FbReaderDlg::ID_FONT_COLOUR},
		{FB_READER_BACK_COLOUR,  FbReaderDlg::ID_BACK_COLOUR},
		{FB_READER_FONT_HEADER,  FbReaderDlg::ID_FONT_HEADER},
		{FB_READER_HEAD_COLOUR,  FbReaderDlg::ID_HEAD_COLOUR},
	};

	const size_t idsCount = sizeof(ids) / sizeof(Struct);

	for (size_t i=0; i<idsCount; i++) {
		FbDialog::Assign(ids[i].control, ids[i].param, write);
	}
}

bool FbReaderDlg::Execute(wxWindow* parent)
{
	FbReaderDlg dlg(parent, wxID_ANY, _("Cool Reader options"), wxDefaultPosition, wxDefaultSize);
	dlg.Assign(false);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) dlg.Assign(true);
	return ok;
}

#endif // FB_INCLUDE_READER
