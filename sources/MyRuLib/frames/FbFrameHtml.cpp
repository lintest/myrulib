#include <wx/fs_mem.h>
#include "FbFrameHtml.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbMainMenu.h"
#include "FbParams.h"
#include "FbMainMenu.h"
#include "MyRuLibApp.h"
#include "FbBookEvent.h"
#include "FbViewThread.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include "FbLogoBitmap.h"
#include "res/modify.xpm"
#include "res/delete.xpm"

IMPLEMENT_CLASS(FbFrameHtml, wxSplitterWindow)

BEGIN_EVENT_TABLE(FbFrameHtml, wxSplitterWindow)
	EVT_COMMAND(ID_BOOK_PREVIEW, fbEVT_BOOK_ACTION, FbFrameHtml::OnInfoUpdate)
	EVT_MENU(ID_HTML_SUBMIT, FbFrameHtml::OnSubmit)
	EVT_MENU(ID_HTML_MODIFY, FbFrameHtml::OnModify)
	EVT_MENU(wxID_SAVE, FbFrameHtml::OnSave)
	EVT_HTML_LINK_CLICKED(ID_PREVIEW_CTRL, FbFrameHtml::OnLinkClicked)
	EVT_TEXT_ENTER(ID_HTML_CAPTION, FbFrameHtml::OnEnter)
END_EVENT_TABLE()

FbFrameHtml::FbFrameHtml(wxAuiNotebook * parent, int id)
	: wxSplitterWindow(parent, ID_FRAME_HTML, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER | wxTAB_TRAVERSAL),
		m_id(id), m_md5sum( FbCommonDatabase().GetMd5(id)), m_thread(NULL)
{
	parent->AddPage( this, GetTitle(), true );
	static bool bNotLoaded = true;
	if (bNotLoaded) {
		wxMemoryFSHandler::AddFile(wxT("modify"), wxBitmap(modify_xpm), wxBITMAP_TYPE_PNG);
		wxMemoryFSHandler::AddFile(wxT("delete"), wxBitmap(delete_xpm), wxBITMAP_TYPE_PNG);
		bNotLoaded = false;
	}
	CreateControls();
	UpdateFonts(false);
	Reset();
}

FbFrameHtml::~FbFrameHtml()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
}

void FbFrameHtml::Reset()
{
	FbViewContext ctx;
	ctx.editable = true;
	ctx.vertical = false;
	FbViewItem view(FbViewItem::Book, m_id);
	m_thread = new FbViewThread(this, ctx, view);
	m_thread->Execute();
}

void FbFrameHtml::Load(const wxString & html)
{
	m_info.SetPage(html);
	m_info.SetFocus();
}

void FbFrameHtml::CreateControls()
{
	SetMinimumPaneSize(80);
	SetSashGravity(1);

	m_info.Create(this, ID_PREVIEW_CTRL);

	wxPanel * panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(-1, 80), wxTAB_TRAVERSAL );
	wxBoxSizer * bSizerComment = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer * bSizerSubject = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * staticText = new wxStaticText( panel, wxID_ANY, _("Comment:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	bSizerSubject->Add( staticText, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_Caption.Create( panel, ID_HTML_CAPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSizerSubject->Add( &m_Caption, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_ToolBar.Create( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	m_ToolBar.SetFont(FbParams(FB_FONT_TOOL));
	m_ToolBar.AddTool( ID_HTML_SUBMIT, _("Append"), wxBitmap(add_xpm) );
	m_ToolBar.AddTool( ID_HTML_MODIFY, _("Modify"), wxBitmap(mod_xpm) );
	m_ToolBar.EnableTool(ID_HTML_MODIFY, false);
	m_ToolBar.Realize();

	bSizerSubject->Add( &m_ToolBar, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	bSizerComment->Add( bSizerSubject, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_Comment.Create( panel, ID_HTML_COMMENT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP );
	bSizerComment->Add( &m_Comment, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	panel->SetSizer( bSizerComment );
	panel->Layout();
	bSizerComment->Fit( panel );

	SplitHorizontally(&m_info, panel, GetClientRect().y - 150);
	m_Caption.SetFocus();
}

void FbFrameHtml::Localize(bool bUpdateMenu)
{
//	SetTitle(GetTitle());
//	FbAuiMDIChildFrame::Localize(bUpdateMenu);
}

void FbFrameHtml::OnSave(wxCommandEvent& event)
{
	wxFileDialog dlg (
		this,
		_("Select a file to export report"),
		wxEmptyString,
		wxT("lib_info.html"),
		_("HTML files (*.html; *.htm)|*.html;*.HTML;*.HTM;*.htm|All files (*.*)|*.*"),
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
	bool ok = stmt.ExecuteUpdate();

	if (ok) {
		m_Caption.SetValue(wxEmptyString);
		m_Comment.SetValue(wxEmptyString);
		m_ToolBar.EnableTool(ID_HTML_MODIFY, false);
		m_key.Empty();
		DoUpdate();
	}
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
	bool ok = stmt.ExecuteUpdate();

	if (ok) {
		m_Caption.SetValue(wxEmptyString);
		m_Comment.SetValue(wxEmptyString);
		m_ToolBar.EnableTool(ID_HTML_MODIFY, false);
		m_key.Empty();
		DoUpdate();
	}
}

void FbFrameHtml::OnLinkClicked(wxHtmlLinkEvent& event)
{
	wxString key = event.GetLinkInfo().GetHref();
	if ( event.GetLinkInfo().GetTarget() == wxT("D") )
	{
		int res = wxMessageBox(_("Remove comment?"), _("Confirmation"), wxOK|wxCANCEL);
		if (res == wxOK) DeleteLink(key);
	}
	else if ( event.GetLinkInfo().GetTarget() == wxT("M") )
	{
		ModifyLink(key);
	}
}

void FbFrameHtml::ModifyLink(const wxString &key)
{
	FbLocalDatabase database;
	wxString sql = wxT("SELECT id, caption, comment FROM comments WHERE id=") + key;
	wxSQLite3ResultSet res = database.ExecuteQuery(sql);
	if (res.NextRow()) {
		m_key = res.GetString(0);
		m_Caption.SetValue( res.GetString(1) );
		m_Comment.SetValue( res.GetString(2) );
		m_ToolBar.EnableTool(ID_HTML_MODIFY, true);
	}
}

void FbFrameHtml::DeleteLink(const wxString &key)
{
	FbLocalDatabase database;
	wxString sql = wxT("DELETE FROM comments WHERE id=") + key;
	bool ok = database.ExecuteUpdate(sql);

	if (ok && m_key == key) {
		m_key.Empty();
		m_ToolBar.EnableTool(ID_HTML_MODIFY, false);
	}
	DoUpdate();
}

void FbFrameHtml::DoUpdate()
{
	Reset();
	FbFolderEvent(ID_UPDATE_FOLDER, 1, FT_COMMENT).Post();
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_UPDATE_BOOK, m_id).Post();
}

void FbFrameHtml::UpdateFonts(bool refresh)
{
	if (refresh) m_info.SetPage(wxEmptyString);
	m_info.UpdateFont(refresh);
	Reset();
}

