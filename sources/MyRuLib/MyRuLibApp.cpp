#include "MyRuLibApp.h"
#include <wx/app.h>
#include <wx/fs_inet.h>
#include <wx/fs_mem.h>
#include "FbConst.h"
#include "FbDataPath.h"
#include "FbMainFrame.h"
#include "FbLogStream.h"
#include "FbLocale.h"
#include "FbParams.h"
#include "FbGenres.h"
#include "dialogs/FbDataOpenDlg.h"
#include "FbCollection.h"

IMPLEMENT_APP(MyRuLibApp)

BEGIN_EVENT_TABLE(MyRuLibApp, wxApp)
	EVT_FB_IMAGE(wxID_ANY, MyRuLibApp::OnImageEvent)
END_EVENT_TABLE()

wxCriticalSection MyRuLibApp::sm_section;

MyRuLibApp::MyRuLibApp()
	:m_locale(NULL), m_collection(NULL), m_downloader(NULL)
{
}

void MyRuLibApp::StartDownload()
{
	if (m_downloader) {
		m_downloader->Signal();
	} else {
		m_downloader = new FbDownloader;
		m_downloader->Execute();
		m_downloader->Signal();
	}
}

void MyRuLibApp::StopDownload()
{
	if (m_downloader) {
		m_downloader->Close();
		m_downloader->Signal();
		m_downloader->Delete();
		m_downloader = NULL;
	}
}

void MyRuLibApp::Localize()
{
	wxLanguage language = (wxLanguage) (int) FbParams(FB_LANG_LOCALE);
	if (m_locale && m_locale->GetLanguage() == language) return;

	wxDELETE(m_locale);
	m_locale = new FbLocale;
	m_locale->Init(language);

	FbMainFrame * frame = wxDynamicCast(GetTopWindow(), FbMainFrame);
	if (frame) frame->Localize(language);

	FbGenres::Init();
}

bool MyRuLibApp::OnInit()
{
	wxRegisterId(ID_MENU_HIGHEST);

	#ifdef FB_SYSLOG_LOGGING
	wxLog::SetActiveTarget(new FbLogSyslog);
	#endif // FB_SYSLOG_LOGGING

	FbCollection::LoadConfig();
	Localize();

	#ifndef FB_SYSLOG_LOGGING
	wxLog::SetActiveTarget(new FbLogStream);
	#endif // FB_SYSLOG_LOGGING

	#ifdef __WXDEBUG__
	wxLog::SetVerbose(true);
	#endif // __WXDEBUG__

	wxFileName filename = GetDatabaseFile();
	if (!filename.IsOk()) {
		wxString datafile = FbDataOpenDlg::Execute(NULL);
		if (!datafile) return false;
		filename = datafile;
	}

	filename.Normalize();
	OpenDatabase(filename.GetFullPath());

	::wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxMemoryFSHandler);
	wxFileSystem::AddHandler(new wxInternetFSHandler);

	LoadBlankImage();

	FbMainFrame * frame = new FbMainFrame;
	SetTopWindow(frame);
	frame->Show();

	return true;
}

void MyRuLibApp::LoadBlankImage()
{
	/* XPM */
	static const char * blank_xpm[] = {
	/* columns rows colors chars-per-pixel */
	"1 1 1 1",
	"- c None",
	/* pixels */
	"-",
	};

	wxBitmap bitmap(blank_xpm);
	wxMemoryFSHandler::AddFile(wxT("blank"), bitmap, wxBITMAP_TYPE_PNG);
}

wxFileName MyRuLibApp::GetDatabaseFile()
{
	wxFileName filename = FbStandardPaths().GetExecutablePath();
	filename.SetExt(wxT("db"));

	if (wxGetApp().argc > 1) {
		wxString arg = wxGetApp().argv[1];
		if (wxFileName::DirExists(arg)) {
			filename.SetPath(arg);
			return filename;
		}
		return wxFileName(arg);
	}

	if (filename.FileExists()) return filename;

	wxString recent = FbCollection::GetParamStr(FB_RECENT_0);
	if (!recent.IsEmpty()) {
		wxFileName filename = recent;
		if (filename.FileExists()) return filename;
	}

	return wxFileName();
}

int MyRuLibApp::OnExit()
{
	StopDownload();
	wxDELETE(m_locale);
	wxDELETE(m_collection);
	return wxApp::OnExit();
}

bool MyRuLibApp::OpenDatabase(const wxString &filename)
{
	FbCollection * collection = new FbCollection(filename);
	bool ok = collection->IsOk();
	if (ok) {
		SetLibFile(filename);
		wxCriticalSectionLocker locker(sm_section);
		wxDELETE(m_collection);
		m_collection = collection;
	} else {
		delete collection;
	}
	if (ok) {
		FbParamItem::AddRecent(filename, FbParams(DB_LIBRARY_TITLE));
		UpdateLibPath();
	}
	return ok;
}

FbCollection * MyRuLibApp::GetCollection()
{
	wxCriticalSectionLocker locker(sm_section);
	return m_collection;
}

const wxString MyRuLibApp::GetLibFile() const
{
	wxCriticalSectionLocker locker(sm_section);
	return m_LibFile;
}

const wxString MyRuLibApp::GetLibPath() const
{
	wxCriticalSectionLocker locker(sm_section);
	return m_LibPath;
}

void MyRuLibApp::SetLibFile(const wxString & filename)
{
	{
		wxCriticalSectionLocker locker(sm_section);
		m_LibFile = filename;
	}
}

void MyRuLibApp::UpdateLibPath()
{
	wxFileName dirname = GetLibFile();
	dirname.SetPath(FbParams(DB_LIBRARY_DIR));
	if (dirname.IsRelative()) {
		wxFileName filename = GetLibFile();
		dirname.MakeAbsolute(filename.GetPath());
	}
	wxCriticalSectionLocker locker(sm_section);
	m_LibPath = dirname.GetPath();
}

void MyRuLibApp::OnImageEvent(FbImageEvent & event)
{
	FbViewData::Push(event.GetString(), event.GetImage());
	FbMainFrame * frame = wxDynamicCast(GetTopWindow(), FbMainFrame);
	if (frame) frame->UpdateBook(event.GetInt());
}

#ifdef __WXMSW__

static bool Wheel(wxEvent & event)
{
	wxMouseEvent * mouse = wxDynamicCast(&event, wxMouseEvent);
	if (!mouse) return false;

	wxWindow * object = wxDynamicCast(mouse->GetEventObject(), wxWindow);
	if (!object) return false;

	wxPoint pt = object->ClientToScreen(mouse->GetPosition());
	wxWindow * window = wxFindWindowAtPoint(pt);
	if (!window || window == object) return false;

	if (window->HasScrollbar(wxVERTICAL)) {
		int delta = mouse->GetLinesPerAction();
		if (window->GetId() == ID_FRAME_READ) delta = 1;
		if (mouse->GetWheelRotation() > 0 ) delta *= -1;
		window->ScrollLines(delta);
		return true;
	}

	return false;
}

int MyRuLibApp::FilterEvent(wxEvent & event)
{
	if (event.GetEventType() == wxEVT_MOUSEWHEEL && Wheel(event)) return 1;
	return wxApp::FilterEvent(event);
}

#endif // __WXMSW__
