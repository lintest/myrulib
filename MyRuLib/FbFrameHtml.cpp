#include <wx/fs_mem.h>
#include "FbFrameHtml.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbMainMenu.h"
#include "BaseThread.h"
#include "MyRuLibApp.h"
#include "InfoCash.h"
#include "FbBookEvent.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include "res/modify.xpm"
#include "res/delete.xpm"

BEGIN_EVENT_TABLE(FbFrameHtml, FbAuiMDIChildFrame)
	EVT_MENU(ID_HTML_SUBMIT, FbFrameHtml::OnSubmit)
	EVT_MENU(ID_HTML_MODIFY, FbFrameHtml::OnModify)
	EVT_MENU(ID_BOOKINFO_UPDATE, FbFrameHtml::OnInfoUpdate)
	EVT_MENU(wxID_SAVE, FbFrameHtml::OnSave)
	EVT_HTML_LINK_CLICKED(ID_HTML_DOCUMENT, FbFrameHtml::OnLinkClicked)
	EVT_TEXT_ENTER(ID_HTML_CAPTION, FbFrameHtml::OnEnter)
END_EVENT_TABLE()

wxString FbFrameHtml::GetMd5sum(const int id)
{
	FbCommonDatabase database;
	wxString sql = wxT("SELECT md5sum FROM books WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet res = stmt.ExecuteQuery();
	if (res.NextRow())
		return res.GetString(0);
	else
		return wxEmptyString;
}

FbFrameHtml::FbFrameHtml(wxAuiMDIParentFrame * parent, int id)
	:m_id(id), m_md5sum(GetMd5sum(id))
{
	FbAuiMDIChildFrame::Create(parent, ID_FRAME_HTML, _("Комментарии"));
	static bool bNotLoaded = true;
	if (bNotLoaded) {
		wxMemoryFSHandler::AddFile(wxT("modify"), wxBitmap(modify_xpm), wxBITMAP_TYPE_PNG);
		wxMemoryFSHandler::AddFile(wxT("delete"), wxBitmap(delete_xpm), wxBITMAP_TYPE_PNG);
		bNotLoaded = false;
	}
	CreateControls();
	UpdateFonts(false);
	InfoCash::UpdateInfo(this, m_id, false, true);
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

	m_info.Create(splitter, ID_HTML_DOCUMENT);

	wxPanel * panel = new wxPanel( splitter, wxID_ANY, wxDefaultPosition, wxSize(-1, 80), wxTAB_TRAVERSAL );
	wxBoxSizer * bSizerComment = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerSubject;
	bSizerSubject = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * staticText = new wxStaticText( panel, wxID_ANY, wxT("Комментарий:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	bSizerSubject->Add( staticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_Caption.Create( panel, ID_HTML_CAPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerSubject->Add( &m_Caption, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_ToolBar.Create( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER|wxTB_NOICONS|wxTB_TEXT );
	m_ToolBar.SetFont(FbParams::GetFont(FB_FONT_TOOL));
	m_ToolBar.AddTool( ID_HTML_SUBMIT, wxT("Добавить"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar.AddTool( ID_HTML_MODIFY, wxT("Изменить"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar.EnableTool(ID_HTML_MODIFY, false);
	m_ToolBar.Realize();

	bSizerSubject->Add( &m_ToolBar, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	bSizerComment->Add( bSizerSubject, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_Comment.Create( panel, ID_HTML_COMMENT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP );
	bSizerComment->Add( &m_Comment, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	panel->SetSizer( bSizerComment );
	panel->Layout();
	bSizerComment->Fit( panel );

	splitter->SplitHorizontally(&m_info, panel, GetClientRect().y - 150);
	m_Caption.SetFocus();

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
		m_info.SetPage(html);
//		wxMessageBox(html);
	}
}

void FbFrameHtml::DoSubmit()
{
	wxString caption = m_Caption.GetValue();
	wxString comment = m_Comment.GetValue();
	if ( caption.IsEmpty() && comment.IsEmpty() ) return;

	wxString sql = wxT("INSERT INTO comments(id, md5sum, posted, caption, comment) VALUES (?,?,?,?,?)");

	FbLocalDatabase database;
	int key = database.NewId(FB_NEW_COMMENT);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, key);
	stmt.Bind(2, m_md5sum);
	stmt.Bind(3, wxDateTime::Now().FormatISODate() + wxT(" ") + wxDateTime::Now().FormatISOTime());
	stmt.Bind(4, caption);
	stmt.Bind(5, comment);
	stmt.ExecuteUpdate();

	m_Caption.SetValue(wxEmptyString);
	m_Comment.SetValue(wxEmptyString);
	m_ToolBar.EnableTool(ID_HTML_MODIFY, false);

	m_key.Empty();

	DoUpdate();
}

void FbFrameHtml::OnSubmit(wxCommandEvent& event)
{
	DoSubmit();
}

void FbFrameHtml::OnModify(wxCommandEvent& event)
{
	DoModify();
}

void FbFrameHtml::OnEnter(wxCommandEvent& event)
{
	if (m_key.IsEmpty())
		DoSubmit();
	else
		DoModify();
}

void FbFrameHtml::DoModify()
{
	if (m_key.IsEmpty()) return;

	wxString caption = m_Caption.GetValue();
	wxString comment = m_Comment.GetValue();
	if ( caption.IsEmpty() && comment.IsEmpty() ) return;

	wxString sql = wxT("UPDATE comments SET posted=?, caption=?, comment=? WHERE id=?");

	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, wxDateTime::Now().FormatISODate() + wxT(" ") + wxDateTime::Now().FormatISOTime());
	stmt.Bind(2, caption);
	stmt.Bind(3, comment);
	stmt.Bind(4, m_key);
	stmt.ExecuteUpdate();

	m_Caption.SetValue(wxEmptyString);
	m_Comment.SetValue(wxEmptyString);
	m_ToolBar.EnableTool(ID_HTML_MODIFY, false);

	m_key.Empty();

	DoUpdate();
}

void FbFrameHtml::OnLinkClicked(wxHtmlLinkEvent& event)
{
	FbLocalDatabase database;
	wxString key = event.GetLinkInfo().GetHref();

	if ( event.GetLinkInfo().GetTarget() == wxT("D") )
	{
		int res = wxMessageBox(_("Удалить комментарий?"), _("Подтверждение"), wxOK|wxCANCEL);
		if (res != wxOK) return;

		wxString sql = wxT("DELETE FROM comments WHERE id=") + key;
		database.ExecuteUpdate(sql);

		if (m_key == key) {
			m_key.Empty();
			m_ToolBar.EnableTool(ID_HTML_MODIFY, false);
		}
		DoUpdate();
	}
	else if ( event.GetLinkInfo().GetTarget() == wxT("M") )
	{
		wxString sql = wxT("SELECT id, caption, comment FROM comments WHERE id=") + key;
		wxSQLite3ResultSet res = database.ExecuteQuery(sql);
		if (res.NextRow()) {
			m_key = res.GetString(0);
			m_Caption.SetValue( res.GetString(1) );
			m_Comment.SetValue( res.GetString(2) );
			m_ToolBar.EnableTool(ID_HTML_MODIFY, true);
		}
	}
}

void FbFrameHtml::DoUpdate()
{
	InfoCash::UpdateInfo(this, m_id, false, true);
	FbFolderEvent(ID_UPDATE_FOLDER, 1, FT_COMMENT).Post();
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_UPDATE_BOOK, m_id).Post();
}

void FbFrameHtml::UpdateFonts(bool refresh)
{
	if (refresh) m_info.SetPage(wxEmptyString);
	FbAuiMDIChildFrame::UpdateFont(&m_info, refresh);
	if (refresh) InfoCash::UpdateInfo(this, m_id, false, true);
}
