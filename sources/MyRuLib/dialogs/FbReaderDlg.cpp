#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "FbReaderDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "controls/FbTreeView.h"
#include "FbViewerDlg.h"
#include "FbCollection.h"
#include "FbDataPath.h"
#include "controls/FbChoiceCtrl.h"
#include "controls/FbComboBox.h"
#include "frames/FbCoolReader.h"
#include "FbLogoBitmap.h"
#include "MyRuLibApp.h"

#ifdef FB_INCLUDE_READER

//-----------------------------------------------------------------------------
//  FbReaderDlg::PanelMain
//-----------------------------------------------------------------------------

FbReaderDlg::PanelMain::PanelMain(wxWindow *parent, wxArrayString & fonts)
	: wxPanel(parent)
{
	wxFlexGridSizer * fgSizerList = new wxFlexGridSizer( 2 );
	fgSizerList->AddGrowableCol( 1 );
	fgSizerList->SetFlexibleDirection( wxBOTH );
	fgSizerList->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText * stTitle;
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Main font"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxComboBox * cbFontName = new wxComboBox( this, ID_READER_FONT_NAME );
	cbFontName->Append(fonts);
	fgSizerList->Add( cbFontName, 0, wxALL|wxEXPAND, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Font size"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxSpinCtrl * scFontSize = new wxSpinCtrl( this, ID_READER_FONT_SIZE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 99, 0 );
	fgSizerList->Add( scFontSize, 0, wxALL, 5 );

	stTitle = new wxStaticText( this, wxID_ANY, _("Interline space"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxSpinCtrl * scInterline = new wxSpinCtrl( this, ID_READER_INTERLINE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 50, 200, 100 );
	fgSizerList->Add( scInterline, 0, wxALL, 5 );

	stTitle = new wxStaticText( this, wxID_ANY, _("Font colour"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxColourPickerCtrl * cpFont = new wxColourPickerCtrl( this, ID_READER_FONT_COLOUR);
	fgSizerList->Add( cpFont, 0, wxALL, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Background colour"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxColourPickerCtrl * cpBack = new wxColourPickerCtrl( this, ID_READER_BACK_COLOUR);
	fgSizerList->Add( cpBack, 0, wxALL, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Hyphenation"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxCheckBox * checkbox = new wxCheckBox( this, ID_READER_HYPHENATION, _("Use algorithmic hyphenation"));
	fgSizerList->Add( checkbox, 0, wxEXPAND|wxALL, 5 );

	this->SetSizer( fgSizerList );
	this->Layout();
}

//-----------------------------------------------------------------------------
//  FbReaderDlg::PanelPage
//-----------------------------------------------------------------------------

FbReaderDlg::PanelPage::PanelPage(wxWindow *parent, wxArrayString & fonts)
	: wxPanel(parent)
{
	wxFlexGridSizer * fgSizerList = new wxFlexGridSizer( 2 );
	fgSizerList->AddGrowableCol( 1 );
	fgSizerList->SetFlexibleDirection( wxBOTH );
	fgSizerList->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText * stTitle;
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Page header"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxCheckBox * checkbox = new wxCheckBox( this, ID_HEADER_SHOW_HEADER, _("Show page header"));
	fgSizerList->Add( checkbox, 0, wxEXPAND|wxALL, 5 );

	stTitle = new wxStaticText( this, wxID_ANY, _("Header font"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxComboBox * cbFontName = new wxComboBox( this, ID_HEADER_FONT_NAME );
	cbFontName->Append(fonts);
	fgSizerList->Add( cbFontName, 0, wxALL|wxEXPAND, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("Font size"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxSpinCtrl * scFontSize = new wxSpinCtrl( this, ID_HEADER_FONT_SIZE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 99, 0 );
	fgSizerList->Add( scFontSize, 0, wxALL, 5 );

	stTitle = new wxStaticText( this, wxID_ANY, _("Font colour"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxColourPickerCtrl * cpFont = new wxColourPickerCtrl( this, ID_HEADER_FONT_COLOUR);
	fgSizerList->Add( cpFont, 0, wxALL, 5 );
	
	stTitle = new wxStaticText( this, wxID_ANY, _("View mode"));
	stTitle->Wrap( -1 );
	fgSizerList->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	checkbox = new wxCheckBox( this, ID_READER_PAGE_COUNT, _("Display two pages"));
	fgSizerList->Add( checkbox, 0, wxEXPAND|wxALL, 5 );

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
	
	wxArrayString fonts;
	FbCoolReader::GetFonts(fonts);

	wxNotebook * notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	notebook->AddPage( new PanelMain(notebook, fonts), _("General"), true );
	notebook->AddPage( new PanelPage(notebook, fonts), _("Page"), false );
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
		{ FB_READER_FONT_COLOUR  , FbReaderDlg::ID_READER_FONT_COLOUR },
		{ FB_READER_BACK_COLOUR  , FbReaderDlg::ID_READER_BACK_COLOUR },
		{ FB_HEADER_FONT_COLOUR  , FbReaderDlg::ID_HEADER_FONT_COLOUR },
		{ FB_READER_FONT_NAME    , FbReaderDlg::ID_READER_FONT_NAME   },
		{ FB_READER_FONT_SIZE    , FbReaderDlg::ID_READER_FONT_SIZE   },
		{ FB_HEADER_FONT_NAME    , FbReaderDlg::ID_HEADER_FONT_NAME   },
		{ FB_HEADER_FONT_SIZE    , FbReaderDlg::ID_HEADER_FONT_SIZE   },
		{ FB_READER_SHOW_HEADER  , FbReaderDlg::ID_HEADER_SHOW_HEADER },
		{ FB_READER_INTERLINE    , FbReaderDlg::ID_READER_INTERLINE   },
		{ FB_READER_HYPHENATION  , FbReaderDlg::ID_READER_HYPHENATION },
		{ FB_READER_PAGE_COUNT   , FbReaderDlg::ID_READER_PAGE_COUNT  },
	};

	const size_t count = sizeof(ids) / sizeof(Struct);
	for (size_t i = 0; i < count; i++) {
		FbDialog::Assign(ids[i].control, ids[i].param, write);
	}
}

bool FbReaderDlg::Execute(wxWindow* parent)
{
	if (!FbCoolReader::InitCREngine()) return false;
	FbReaderDlg dlg(parent, wxID_ANY, _("Cool Reader options"));
	dlg.Assign(false);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) dlg.Assign(true);
	return ok;
}

#endif // FB_INCLUDE_READER
