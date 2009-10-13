#include "FbFrameInfo.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbMenu.h"
#include "BaseThread.h"
#include "MyRuLibApp.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

FbFrameInfo::FbFrameInfo(wxAuiMDIParentFrame * parent)
    :wxAuiMDIChildFrame(parent, ID_FRAME_INFO, _("Информация"))
{
	CreateControls();
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

wxMenuBar * FbFrameInfo::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;
	FbMenu * menu;

	menu = new FbMenu;
	menu->AppendImg(wxID_NEW, _("Добавить файл"), wxART_NEW);
	menu->AppendImg(wxID_OPEN, _("Добавить директорию"), wxART_FOLDER_OPEN);
	menu->AppendSeparator();
	menu->AppendImg(wxID_EXIT, _("Выход\tAlt+F4"), wxART_QUIT);
	menuBar->Append(menu, _("&Файл"));

	menu = new FbMenu;
	menu->AppendImg(ID_MENU_SEARCH, _("Расширенный"), wxART_FIND);
	menu->AppendSeparator();
	menu->Append(ID_MENU_AUTHOR, _("по Автору"));
	menu->Append(ID_MENU_TITLE, _("по Заголовку"));
	menu->Append(ID_FRAME_GENRES, _("по Жанрам"));
	menu->AppendSeparator();
	menu->Append(ID_FRAME_FAVOUR, _("Избранное"));
	menuBar->Append(menu, _("&Поиск"));

	menu = new FbMenu;
	menu->Append(ID_MENU_DB_INFO, _("Информация о коллекции"));
	menu->Append(ID_MENU_VACUUM, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	menuBar->Append(menu, _("&Сервис"));

	menu = new FbMenu;
	menu->Append(ID_OPEN_WEB, _("Официальный сайт"));
	menu->AppendImg(wxID_ABOUT, _("О программе…"), wxART_HELP_PAGE);
	menuBar->Append(menu, _("&?"));

	return menuBar;
}

class FbSizeFunction : public wxSQLite3ScalarFunction
{
	public:
		FbSizeFunction(): m_count(0) {};
		virtual void Execute(wxSQLite3FunctionContext& ctx);
		int64_t m_count;
};

void FbSizeFunction::Execute(wxSQLite3FunctionContext& ctx)
{
    int argCount = ctx.GetArgCount();
    if (argCount != 1) return;
    int size = ctx.GetInt(0);
    m_count += size;
    ctx.SetResult(false);
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
	private:
		FbCommonDatabase m_database;
		FbSizeFunction m_size;
		wxString m_html;
};

void FrameInfoThread::WriteTitle()
{
    m_html += _("<HTML><HEAD><TITLE>Информация о коллекции</TITLE></HEAD><BODY><CENTER>");
}

wxString FrameInfoThread::GetDate(const int number)
{
    int dd = number % 100;
    int mm = number / 100 % 100;
    int yyyy = number / 10000 % 100 + 2000;
    return wxString::Format(wxT("%02d.%02d.%04d"), dd, mm, yyyy);
}

void FrameInfoThread::WriteCount()
{
    m_html += wxT("<TABLE>");
    m_html += wxT("<TR><TD colspan=2 align=center>Информация о коллекции</TD></TR>");
    m_html += wxT("<TR><TD colspan=2 align=center>");
    m_html += wxString::Format(_("<B>%s</B>"), FbParams::GetText(DB_LIBRARY_TITLE).c_str());
    m_html += wxT("</TD></TR>");

    wxString min, max;

	{
		wxString sql = (wxT("SELECT COUNT(DISTINCT id), MIN(created), MAX(created) FROM books"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			min = GetDate(result.GetInt(1));
			max = GetDate(result.GetInt(2));
			m_html += wxString::Format(_("<TR><TD>Общее количество книг:</TD><TD align=center>%d</TD></TR>"), result.GetInt(0));
		}
	}

    {
		wxString sql = (wxT("SELECT COUNT(id) FROM authors"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			m_html += wxString::Format(_("<TR><TD>Количество авторов:</TD><TD align=center>%d</TD></TR>"), result.GetInt(0));
		}
    }

	{
/*
		m_database.CreateFunction(wxT("SIZE"), 1, m_size);
		wxString sql = (wxT("SELECT file_size FROM (SELECT DISTINCT id, file_size AS file_size FROM books) AS books WHERE SIZE(file_size)"));
		m_database.ExecuteQuery(sql);
		m_html += wxString::Format(_("<TR><TD>Размер библиотеки, байт:</TD><TD align=center>%d</TD></TR>"), m_size.m_count / 1024 / 1024);
*/
		wxString sql = (wxT("SELECT SUM(file_size)/1024/1024 FROM (SELECT DISTINCT id, file_size FROM books) AS books"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		if (result.NextRow()) {
			m_html += wxString::Format(_("<TR><TD>Общий размер файлов, Мб:</TD><TD align=center>%d</TD></TR>"), result.GetInt(0));
		}
	}

	m_html += wxString::Format(_("<TR><TD>Первое поступление:</TD><TD align=center>%s г.</TD></TR>"), min.c_str());
	m_html += wxString::Format(_("<TR><TD>Последнее поступление:</TD><TD align=center>%s г.</TD></TR>"), max.c_str());

    m_html += wxT("</TABLE>");
}

void FrameInfoThread::WriteTypes()
{
    m_html += wxT("<BR><BR>");
    m_html += wxT("<TABLE border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><TR><TD>");
    m_html += wxT("<TABLE border=0 cellspacing=1 cellpadding=5 width=100%>");
    m_html += wxT("<TR>");
    m_html += wxT("<TD bgcolor=#FFFFFF><B>Тип файла</B></TD>");
    m_html += wxT("<TD bgcolor=#FFFFFF><B>Количество</B></TD>");
    m_html += wxT("<TD bgcolor=#FFFFFF><B>Размер, Кб</B></TD>");
    m_html += wxT("</TR>");

    wxString min, max;

	{
		wxString sql = (wxT("\
			SELECT file_type, COUNT(DISTINCT id) AS id, SUM(file_size)/1024 \
			FROM (SELECT DISTINCT id, file_type, file_size FROM books) AS books \
			GROUP BY file_type ORDER BY id DESC \
		"));
		wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
		while (result.NextRow()) {
			m_html += wxString::Format(_("<TR><TD bgcolor=#FFFFFF>%s</TD><TD align=right bgcolor=#FFFFFF>%d</TD><TD align=right bgcolor=#FFFFFF>%d</TD></TR>"), result.GetString(0).c_str(), result.GetInt(1), result.GetInt(2));
		}
	}

    m_html += wxT("</TABLE>");
    m_html += wxT("</TD></TR></TABLE>");
}

void * FrameInfoThread::Entry()
{
//    wxCriticalSectionLocker enter(sm_queue);

	try {
		WriteTitle();
		WriteCount();
		WriteTypes();
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	m_html += wxT("</CENTER></BODY></HTML>");

	wxCommandEvent event(fbEVT_BOOK_ACTION, ID_DATABASE_INFO);
	event.SetString(m_html);
	wxPostEvent(wxGetApp().GetTopWindow(), event);

    return NULL;
}

void FbFrameInfo::Execute()
{
	wxThread * thread = new FrameInfoThread;
	if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
}
