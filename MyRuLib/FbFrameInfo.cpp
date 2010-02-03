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

BEGIN_EVENT_TABLE(FbFrameInfo, wxAuiMDIChildFrame)
	EVT_MENU(wxID_SAVE, FbFrameInfo::OnSave)
END_EVENT_TABLE()

FbFrameInfo::FbFrameInfo(wxAuiMDIParentFrame * parent)
{
	FbAuiMDIChildFrame::Create(parent, ID_FRAME_INFO, _("Информация"));
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
	SetMenuBar(new FbMainMenu);

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

	m_html += wxT("<TABLE>");
	m_html += wxT("<TR><TD colspan=2 align=center>Информация о коллекции</TD></TR>");
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

	DoStep(_("Общее количество"));
	{
		wxString sql = (wxT("SELECT COUNT(id), MIN(created), MAX(created), SUM(file_size)/1024/1024 FROM (SELECT DISTINCT id, created, file_size FROM books) AS books"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			min = GetDate(result.GetInt(1));
			max = GetDate(result.GetInt(2));
			sum = F(result.GetInt(3));
			m_html += wxString::Format(_("<TR><TD>Общее количество книг:</TD><TD align=right>%s</TD></TR>"), F(result.GetInt(0)).c_str());
		}
	}
	DoStep(_("Подсчет авторов"));
	{
		wxString sql = (wxT("SELECT COUNT(id) FROM authors WHERE id<>0"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			m_html += wxString::Format(_("<TR><TD>Количество авторов:</TD><TD align=right>%s</TD></TR>"), F(result.GetInt(0)).c_str());
		}
	}
	m_html += wxString::Format(_("<TR><TD>Суммарный размер файлов, Мб:</TD><TD align=right>%s</TD></TR>"), sum.c_str());
	m_html += wxString::Format(_("<TR><TD>Первое поступление:</TD><TD align=right>%s г.</TD></TR>"), min.c_str());
	m_html += wxString::Format(_("<TR><TD>Последнее поступление:</TD><TD align=right>%s г.</TD></TR>"), max.c_str());

	m_html += wxT("</TABLE>");
}

void FrameInfoThread::WriteTypes()
{
	wxString colourBack = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
	wxString colourGrid = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW).GetAsString(wxC2S_HTML_SYNTAX);

	m_html += wxT("<BR><BR>");
	m_html += wxString::Format(wxT("<TABLE border=0 cellspacing=0 cellpadding=0 bgcolor=%s><TR><TD>"), colourGrid.c_str());
	m_html += wxT("<TABLE border=0 cellspacing=1 cellpadding=5 width=100%>");
	m_html += wxT("<TR>");
	m_html += wxString::Format(wxT("<TD bgcolor=%s><B>Тип файла</B></TD>"), colourBack.c_str());
	m_html += wxString::Format(wxT("<TD bgcolor=%s><B>Количество</B></TD>"), colourBack.c_str());
	m_html += wxString::Format(wxT("<TD bgcolor=%s><B>Размер, Кб</B></TD>"), colourBack.c_str());
	m_html += wxT("</TR>");

	DoStep(_("Типы файлов"));
	{
		wxString sql = (wxT("\
			SELECT file_type, COUNT(DISTINCT id) AS id, SUM(file_size)/1024 \
			FROM (SELECT DISTINCT id, file_type, file_size FROM books) AS books \
			GROUP BY file_type ORDER BY id DESC \
		"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		while (result.NextRow()) {
			m_html += wxT("<TR>");
			m_html += wxString::Format(_("<TD bgcolor=%s>%s</TD>"), colourBack.c_str(), result.GetString(0).c_str());
			m_html += wxString::Format(_("<TD align=right bgcolor=%s>%s</TD>"), colourBack.c_str(), F(result.GetInt(1)).c_str());
			m_html += wxString::Format(_("<TD align=right bgcolor=%s>%s</TD>"), colourBack.c_str(), F(result.GetInt(2)).c_str());
			m_html += wxT("</TR>");
		}
	}

	m_html += wxT("</TABLE>");
	m_html += wxT("</TD></TR></TABLE>");
}

void * FrameInfoThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	DoStart(3, _("Информация о коллекции"));

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

void FbFrameInfo::UpdateFonts(bool refresh)
{
	FbAuiMDIChildFrame::UpdateFont(&m_info, refresh);
}
