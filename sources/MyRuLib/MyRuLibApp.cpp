#include "MyRuLibApp.h"
#include <wx/app.h>
#include <wx/fs_inet.h>
#include <wx/fs_mem.h>
#include "FbDataPath.h"
#include "FbMainFrame.h"
#include "FbLogStream.h"
#include "FbLocale.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "FbDataOpenDlg.h"
#include "FbUpdateThread.h"
#include "FbCollection.h"

IMPLEMENT_APP(MyRuLibApp)

BEGIN_EVENT_TABLE(MyRuLibApp, wxApp)
	EVT_FB_IMAGE(wxID_ANY, MyRuLibApp::OnImageEvent)
END_EVENT_TABLE()

MyRuLibApp::MyRuLibApp()
    :m_locale(NULL), m_collection(NULL), m_downloader(NULL)
{
}

MyRuLibApp::~MyRuLibApp()
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
	wxLanguage language = (wxLanguage) FbParams::GetValue(FB_LANG_LOCALE);
	if (m_locale && m_locale->GetLanguage() == language) return;

	wxDELETE(m_locale);
    m_locale = new FbLocale;
    m_locale->Init(language);

    FbMainFrame * frame = wxDynamicCast(wxGetApp().GetTopWindow(), FbMainFrame);
    if (frame) frame->Localize(language);
}

bool MyRuLibApp::OnInit()
{
	FbConfigDatabase config;
	config.Open();
	FbParams().LoadParams(false);
    Localize();

	OpenLog();
	#ifdef __WXDEBUG__
	wxLog::SetVerbose(true);
	#endif // __WXDEBUG__

	wxFileName filename = GetDatabaseFile(config);
	if (!filename.IsOk()) {
		wxString datafile;
		bool ok = FbDataOpenDlg::Execute(NULL, datafile);
		if (!ok) return false;
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

wxFileName MyRuLibApp::GetDatabaseFile(FbDatabase &config)
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

	wxString recent = config.GetText(FB_RECENT_0);
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

void MyRuLibApp::OpenLog()
{
	wxFileName logname = FbDatabase::GetConfigName();
	logname.SetExt(wxT("log"));
	wxLog * logger = new FbLogStream(logname.GetFullPath());
	wxLog::SetActiveTarget(logger);
}

bool MyRuLibApp::OpenDatabase(const wxString &filename)
{
	FbCollection * collection = new FbCollection(filename);
	if (collection->IsOk()) {
		SetAppData(filename);
		FbParams params;
		params.LoadParams();
		params.AddRecent(filename, FbParams::GetText(DB_LIBRARY_TITLE));
		wxDELETE(m_collection);
		m_collection = collection;
		return true;
	} else {
		delete collection;
		return false;
	}
}

const wxString MyRuLibApp::GetAppData()
{
	wxCriticalSectionLocker locker(m_section);
	return m_datafile;
}

const wxString MyRuLibApp::GetAppPath()
{
	wxCriticalSectionLocker locker(m_section);
	return wxFileName(m_datafile).GetPath();
}

const wxString MyRuLibApp::GetLibPath()
{
	wxFileName dirname = FbParams::GetText(DB_LIBRARY_DIR);
	if (dirname.IsRelative()) {
		wxFileName datafile = GetAppData();
		dirname.MakeAbsolute(datafile.GetPath());
	}
	return dirname.GetFullPath();
}

void MyRuLibApp::SetAppData(const wxString &filename)
{
	wxCriticalSectionLocker locker(m_section);
	m_datafile = filename;
}

void MyRuLibApp::OnImageEvent(FbImageEvent & event)
{
	FbViewData::Push(event.GetString(), event.GetImage());
}
