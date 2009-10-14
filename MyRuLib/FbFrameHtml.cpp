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

BEGIN_EVENT_TABLE(FbFrameHtml, wxAuiMDIChildFrame)
    EVT_MENU(ID_BOOKINFO_UPDATE, FbFrameHtml::OnInfoUpdate)
    EVT_MENU(wxID_SAVE, FbFrameHtml::OnSave)
END_EVENT_TABLE()

FbFrameHtml::FbFrameHtml(wxAuiMDIParentFrame * parent, BookTreeItemData & data)
    :wxAuiMDIChildFrame(parent, ID_FRAME_INFO, _("Комментарии")), m_id(data.GetId()), m_type(data.file_type)
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

	m_info.Create(this);
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add( &m_info, 1, wxEXPAND, 5 );
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
