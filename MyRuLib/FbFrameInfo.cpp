#include "FbFrameInfo.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbMainMenu.h"
#include "BaseThread.h"
#include "MyRuLibApp.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

BEGIN_EVENT_TABLE(FbFrameInfo, FbAuiMDIChildFrame)
	EVT_MENU(wxID_SAVE, FbFrameInfo::OnSave)
END_EVENT_TABLE()

FbFrameInfo::FbFrameInfo(wxAuiMDIParentFrame * parent)
	: FbAuiMDIChildFrame(parent, ID_FRAME_INFO, GetTitle())
{
	CreateControls();
	UpdateFonts(false);
}

void FbFrameInfo::Load(const wxString & html)
{
	m_info.SetPage(html);
	m_info.SetFocus();
}

void FbFrameInfo::CreateControls()
{
	SetMenuBar(CreateMenuBar());

	m_info.Create(this);
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add( &m_info, 1, wxEXPAND, 5 );
	SetSizer(sizer);
	Layout();
}

class FrameInfoThread: public BaseThread
{
	protected:
		virtual void * Entry();
	private:
		void WriteTitle();
		void WriteCount();
		void WriteTypes();
		wxString GetDate(const int number);
		wxString F(const int number);
	private:
		FbCommonDatabase m_database;
		wxString m_html;
};

void FrameInfoThread::WriteTitle()
{
	m_html += _("<CENTER>");

	const wxChar * title = _("Information about collection");

	m_html += wxT("<TABLE>");
	m_html += wxString::Format(wxT("<TR><TD colspan=2 align=center>%s</TD></TR>"), title);
	m_html += wxT("<TR><TD colspan=2 align=center>");
	m_html += wxString::Format(_("<B>%s</B>"), FbParams::GetText(DB_LIBRARY_TITLE).c_str());
	m_html += wxT("</TD></TR>");
	m_html += wxString::Format(_("<TR><TD colspan=2>%s</TD></TR>"), wxGetApp().GetAppData().c_str());
	m_html += wxT("</TABLE>");
}

wxString FrameInfoThread::GetDate(const int number)
{
	int dd = number % 100;
	int mm = number / 100 % 100;
	int yyyy = number / 10000 % 100 + 2000;
	return wxString::Format(wxT("%02d.%02d.%04d"), dd, mm, yyyy);
}

wxString FrameInfoThread::F(const int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return F(hi) + wxT("&nbsp;") + wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

void FrameInfoThread::WriteCount()
{
	m_html += wxT("<TABLE>");

	wxString min, max, sum;

	DoStep(_("Total quantity"));

	{
		wxString sql = (wxT("SELECT COUNT(id), MIN(created), MAX(created), SUM(file_size)/1024/1024 FROM (SELECT DISTINCT id, created, file_size FROM books) AS books"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			min = GetDate(result.GetInt(1));
			max = GetDate(result.GetInt(2));
			sum = F(result.GetInt(3));
			const wxChar * title = _("Total books count:");
			m_html += wxString::Format(_("<TR><TD>%s</TD><TD align=right>%s</TD></TR>"), title, F(result.GetInt(0)).c_str());
		}
	}

	DoStep(_("Authors counting"));

	{
		wxString sql = (wxT("SELECT COUNT(id) FROM authors WHERE id<>0"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			const wxChar * title = _("Authors count:");
			m_html += wxString::Format(_("<TR><TD>%s</TD><TD align=right>%s</TD></TR>"), title, F(result.GetInt(0)).c_str());
		}
	}

	{
		const wxChar * title = _("Total files size, Mb:");
		m_html += wxString::Format(_("<TR><TD>%s</TD><TD align=right>%s</TD></TR>"), title, sum.c_str());
	}

	{
		const wxChar * title = _("First reciept:");
		m_html += wxString::Format(_("<TR><TD>%s</TD><TD align=right>%s г.</TD></TR>"), title, min.c_str());
	}

	{
		const wxChar * title = _("Last reciept:");
		m_html += wxString::Format(_("<TR><TD>%s</TD><TD align=right>%s г.</TD></TR>"), title, max.c_str());
	}

	m_html += wxT("</TABLE>");
}

void FrameInfoThread::WriteTypes()
{
	wxString colourBack = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
	wxString colourGrid = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW).GetAsString(wxC2S_HTML_SYNTAX);

	const wxChar * text = wxT("<TD bgcolor=%s><B>%s</B></TD>");
	const wxChar * msg1 = _("File extension");
	const wxChar * msg2 = _("Count");
	const wxChar * msg3 = _("Size, Kb");

	m_html += wxT("<BR><BR>");
	m_html += wxString::Format(wxT("<TABLE border=0 cellspacing=0 cellpadding=0 bgcolor=%s><TR><TD>"), colourGrid.c_str());
	m_html += wxT("<TABLE border=0 cellspacing=1 cellpadding=5 width=100%>");
	m_html += wxT("<TR>");
	m_html += wxString::Format(text, colourBack.c_str(), msg1);
	m_html += wxString::Format(text, colourBack.c_str(), msg2);
	m_html += wxString::Format(text, colourBack.c_str(), msg3);
	m_html += wxT("</TR>");

	DoStep(_("File types"));
	{
		wxString sql = (wxT("\
			SELECT file_type, COUNT(DISTINCT id) AS id, SUM(file_size)/1024 \
			FROM (SELECT DISTINCT id, file_type, file_size FROM books) AS books \
			GROUP BY file_type ORDER BY id DESC \
		"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		while (result.NextRow()) {
			m_html += wxT("<TR>");
			m_html += wxString::Format(wxT("<TD bgcolor=%s>%s</TD>"), colourBack.c_str(), result.GetString(0).c_str());
			m_html += wxString::Format(wxT("<TD align=right bgcolor=%s>%s</TD>"), colourBack.c_str(), F(result.GetInt(1)).c_str());
			m_html += wxString::Format(wxT("<TD align=right bgcolor=%s>%s</TD>"), colourBack.c_str(), F(result.GetInt(2)).c_str());
			m_html += wxT("</TR>");
		}
	}

	m_html += wxT("</TABLE>");
	m_html += wxT("</TD></TR></TABLE>");
}

void * FrameInfoThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	DoStart(3, _("Collection info"));

	try {
		WriteTitle();
		WriteCount();
		WriteTypes();
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	m_html += wxT("</CENTER>");

	DoFinish();

	FbCommandEvent(fbEVT_BOOK_ACTION, ID_DATABASE_INFO, m_html).Post();

	return NULL;
}

void FbFrameInfo::Execute()
{
	wxThread * thread = new FrameInfoThread;
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}

void FbFrameInfo::OnSave(wxCommandEvent& event)
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

void FbFrameInfo::UpdateFonts(bool refresh)
{
	FbAuiMDIChildFrame::UpdateFont(&m_info, refresh);
}
