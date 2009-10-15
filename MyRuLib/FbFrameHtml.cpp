#include "FbFrameHtml.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbMainMenu.h"
#include "BaseThread.h"
#include "MyRuLibApp.h"
#include "InfoCash.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

BEGIN_EVENT_TABLE(FbFrameHtml, wxAuiMDIChildFrame)
    EVT_MENU(ID_BOOKINFO_UPDATE, FbFrameHtml::OnInfoUpdate)
    EVT_MENU(wxID_SAVE, FbFrameHtml::OnSave)
END_EVENT_TABLE()

FbFrameHtml::FbFrameHtml(wxAuiMDIParentFrame * parent, BookTreeItemData & data)
    :wxAuiMDIChildFrame(parent, ID_FRAME_HTML, _("Комментарии")), m_id(data.GetId()), m_type(data.file_type)
{
	CreateControls();
	InfoCash::UpdateInfo(this, m_id, m_type, false);
}

void FbFrameHtml::Load(const wxString & html)
{
	m_info.SetPage(html);
    m_info.SetFocus();
}

void FbFrameHtml::CreateControls()
{
	SetMenuBar(new FbMainMenu);

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_3D);
	splitter->SetMinimumPaneSize(80);
	splitter->SetSashGravity(1);
	sizer->Add(splitter, 1, wxEXPAND);

	m_info.Create(splitter, wxID_ANY);

	wxPanel * panel = new wxPanel( splitter, wxID_ANY, wxDefaultPosition, wxSize(-1, 80), wxTAB_TRAVERSAL );
	wxBoxSizer * bSizerComment = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerSubject;
	bSizerSubject = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * staticText = new wxStaticText( panel, wxID_ANY, wxT("Комментарий:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	bSizerSubject->Add( staticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_Subject = new wxTextCtrl( panel, ID_HTML_SUBJECT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerSubject->Add( m_Subject, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxToolBar * toolbar = new wxToolBar( panel, ID_HTML_APPEND, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER|wxTB_NOICONS|wxTB_TEXT );
	toolbar->AddTool( wxID_ANY, wxT("Добавить"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	toolbar->Realize();

	bSizerSubject->Add( toolbar, 0, wxALIGN_CENTER_VERTICAL, 5 );

	bSizerComment->Add( bSizerSubject, 0, wxEXPAND, 5 );

	m_Comment = new wxTextCtrl( panel, ID_HTML_COMMENT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP );
	bSizerComment->Add( m_Comment, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	panel->SetSizer( bSizerComment );
	panel->Layout();
	bSizerComment->Fit( panel );

	splitter->SplitHorizontally(&m_info, panel, GetClientRect().y - 150);

	SetSizer(sizer);
	Layout();
}

void FbFrameHtml::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg (
		this,
		_("Выберите файл для экспорта отчета"),
		wxEmptyString,
		wxT("lib_info.html"),
		_("Файы HTML (*.html; *.htm)|*.html;*.HTML;*.HTM;*.htm|Все файлы (*.*)|*.*"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

	if (dlg.ShowModal() == wxID_OK) {
   		wxString html = * m_info.GetParser()->GetSource();
        wxFileOutputStream stream(dlg.GetPath());
        wxTextOutputStream text(stream);
		text.WriteString(html);
	}

}

void FbFrameHtml::OnInfoUpdate(wxCommandEvent& event)
{
	if (event.GetInt() == m_id) {
		wxString html = event.GetString();
		m_info.SetPage(event.GetString());
	}
}
