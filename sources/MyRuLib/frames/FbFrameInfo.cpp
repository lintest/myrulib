#include "FbFrameInfo.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbMainMenu.h"
#include "MyRuLibApp.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

IMPLEMENT_CLASS(FbFrameInfo, FbHtmlWindow)

BEGIN_EVENT_TABLE(FbFrameInfo, FbHtmlWindow)
	EVT_MENU(wxID_SAVE, FbFrameInfo::OnSave)
END_EVENT_TABLE()

FbFrameInfo::FbFrameInfo(wxAuiNotebook * parent, bool select)
	: FbHtmlWindow(parent, ID_FRAME_INFO)
{
	UpdateFonts(false);
	parent->AddPage( this, _("Information"), select );
	Update();
}

void FbFrameInfo::Load(const wxString & html)
{
	SetPage(html);
	SetFocus();
}

class FbFrameInfoThread
	: public FbProgressThread
{
public:
	FbFrameInfoThread(wxEvtHandler * owner)
		: FbProgressThread(owner) {}
protected:
	virtual void * Entry();
private:
	wxString CreateRow(const wxString &info, const wxString &text);
	void WriteTitle();
	void WriteCount();
	void WriteTypes();
	wxString GetDate(const int number);
	wxString F(const wxLongLong &number);
private:
	FbCommonDatabase m_database;
	wxString m_html;
};

void FbFrameInfoThread::WriteTitle()
{
	m_html += wxT("<CENTER>");

	const wxChar * title = _("Information about collection");

	m_html += wxT("<TABLE>");
	m_html += wxString::Format(wxT("<TR><TD colspan=2 align=center>%s</TD></TR>"), title);
	m_html += wxT("<TR><TD colspan=2 align=center>");
	m_html += wxString::Format(wxT("<B>%s</B>"), FbParams(DB_LIBRARY_TITLE).Str().c_str());
	m_html += wxT("</TD></TR>");
	m_html += wxString::Format(wxT("<TR><TD colspan=2>%s</TD></TR>"), wxGetApp().GetLibFile().c_str());
	m_html += wxString::Format(wxT("<TR><TD colspan=2>%s</TD></TR>"), wxGetApp().GetLibPath().c_str());
	m_html += wxT("</TABLE>");
}

wxString FbFrameInfoThread::GetDate(const int number)
{
	int dd = number % 100;
	int mm = number / 100 % 100;
	int yyyy = number / 10000 % 100 + 2000;
	return wxString::Format(wxT("%02d.%02d.%04d"), dd, mm, yyyy);
}

wxString FbFrameInfoThread::F(const wxLongLong &number)
{
	wxLongLong hi = number / 1000;
	long lo = (number % 1000).ToLong();
	if (hi != 0)
		return F(hi) + wxT("&nbsp;") + wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

wxString FbFrameInfoThread::CreateRow(const wxString &info, const wxString &text)
{
	const wxString row = wxT("<TR><TD>%s</TD><TD align=right>%s</TD></TR>");
	return wxString::Format(row, info.c_str(), text.c_str());
}

void FbFrameInfoThread::WriteCount()
{
	m_html += wxT("<TABLE>");

	wxString min, max, sum;

	DoStep(_("Total quantity"));

	{
		wxString sql = (wxT("SELECT COUNT(id), MIN(created), MAX(created), SUM(file_size)/1024/1024 FROM (SELECT DISTINCT id, created, file_size FROM books WHERE deleted IS NULL) AS books"));
		FbSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			min = GetDate(result.GetInt(1));
			max = GetDate(result.GetInt(2));
			sum = F(result.GetInt64(3));
			m_html << CreateRow(_("Total books count:"), F(result.GetInt64(0)));
		}
	}

	DoStep(_("Authors counting"));

	{
		wxString sql = (wxT("SELECT COUNT(id) FROM authors WHERE id<>0"));
		FbSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			wxString count = F(result.GetInt64(0));
			m_html << CreateRow(_("Authors count:"), count);
		}
	}

	m_html << CreateRow(_("Total files size, Mb:"), sum);

	m_html << CreateRow(_("First reciept:"), min);

	m_html << CreateRow(_("Last reciept:"), max);

	m_html << wxT("</TABLE>");
}

void FbFrameInfoThread::WriteTypes()
{
	wxString colourBack = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
	wxString colourGrid = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW).GetAsString(wxC2S_HTML_SYNTAX);

	const wxChar * cell = wxT("<TD bgcolor=%s><B>%s</B></TD>");
	const wxChar * msg1 = _("File extension");
	const wxChar * msg2 = _("Count");
	const wxChar * msg3 = _("Size, Kb");

	m_html += wxT("<BR><BR>");
	m_html += wxString::Format(wxT("<TABLE border=0 cellspacing=0 cellpadding=0 bgcolor=%s><TR><TD>"), colourGrid.c_str());
	m_html += wxT("<TABLE border=0 cellspacing=1 cellpadding=5 width=100%>");
	m_html += wxT("<TR>");
	m_html += wxString::Format(cell, colourBack.c_str(), msg1);
	m_html += wxString::Format(cell, colourBack.c_str(), msg2);
	m_html += wxString::Format(cell, colourBack.c_str(), msg3);
	m_html += wxT("</TR>");

	DoStep(_("File types"));
	{
		wxString sql = (wxT("\
			SELECT file_type, COUNT(id) AS id, SUM(file_size)/1024 \
			FROM (SELECT DISTINCT id, file_type, file_size FROM books WHERE deleted IS NULL) AS books \
			GROUP BY file_type ORDER BY id DESC \
		"));
		FbSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		while (result.NextRow()) {
			const wxString cell = wxT("<TD align=right bgcolor=%s>%s</TD>");
			m_html += wxT("<TR>");
			m_html += wxString::Format(wxT("<TD bgcolor=%s>%s</TD>"), colourBack.c_str(), result.GetString(0).c_str());
			m_html += wxString::Format(cell, colourBack.c_str(), F(result.GetInt64(1)).c_str());
			m_html += wxString::Format(cell, colourBack.c_str(), F(result.GetInt64(2)).c_str());
			m_html += wxT("</TR>");
		}
	}

	m_html += wxT("</TABLE>");
	m_html += wxT("</TD></TR></TABLE>");
}

void * FbFrameInfoThread::Entry()
{
	DoStart(_("Collection info"), 3);

	WriteTitle();
	WriteCount();
	WriteTypes();

	m_html << wxT("</CENTER>");

	DoFinish();

	FbCommandEvent(fbEVT_BOOK_ACTION, ID_DATABASE_INFO, m_html).Post();

	return NULL;
}

void FbFrameInfo::Execute(wxEvtHandler * owner)
{
	(new FbFrameInfoThread(owner))->Execute();
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
		wxString html = * GetParser()->GetSource();
		wxFileOutputStream stream(dlg.GetPath());
		wxTextOutputStream text(stream);
		text.WriteString(html);
	}

}

void FbFrameInfo::UpdateFonts(bool refresh)
{
//	FbAuiMDIChildFrame::UpdateFont(&m_info, refresh);
}
