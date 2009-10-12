#include "FbFrameInfo.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbMenu.h"
#include "BaseThread.h"
#include "MyRuLibApp.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

FbFrameInfo::FbFrameInfo(wxAuiMDIParentFrame * parent, const wxString & html)
    :wxAuiMDIChildFrame(parent, ID_FRAME_INFO, _("Информация"))
{
	CreateControls();
	m_info.SetPage(html);
//	wxRemoveFile(filename);
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

class FrameInfoThread: public BaseThread
{
	protected:
		virtual void * Entry();
	private:
		void WriteTitle();
		void WriteCount();
		wxString GetDate(const int number);
	private:
		FbCommonDatabase m_database;
		wxString m_html;
};

void FrameInfoThread::WriteTitle()
{
    m_html += _("<HTML><HEAD><TITLE>Информация о коллекции</TITLE></HEAD><BODY>");
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
    wxString sql = (wxT("SELECT COUNT(DISTINCT id), MIN(created), MAX(created) FROM books"));
    wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
    if (!result.NextRow()) return ;

    m_html += wxT("<TABLE>");
    m_html += wxT("<TR><TD colspan=2 align=center>Информация о коллекции</TD></TR>");
    m_html += wxT("<TR><TD colspan=2 align=center>");
    m_html += wxString::Format(_("<B>%s</B>"), FbParams::GetText(DB_LIBRARY_TITLE).c_str());
    m_html += wxT("</TD></TR>");

    m_html += wxString::Format(_("<TR><TD>Общее количество книг:</TD><TD align=center>%d</TD></TR>"), result.GetInt(0));
    m_html += wxString::Format(_("<TR><TD>Первое поступление:</TD><TD align=center>%s г.</TD></TR>"),GetDate(result.GetInt(1)).c_str());
    m_html += wxString::Format(_("<TR><TD>Последнее поступление:</TD><TD align=center>%s г.</TD></TR>"),GetDate(result.GetInt(2)).c_str());
    m_html += wxT("</TABLE>");
}

void * FrameInfoThread::Entry()
{
//    wxCriticalSectionLocker enter(sm_queue);

	WriteTitle();
    WriteCount();

	m_html += wxT("</BODY></HTML>");

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
