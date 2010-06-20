#include "MyRuLibApp.h"
#include <wx/app.h>
#include <wx/image.h>
#include <wx/fs_mem.h>
#include <wx/fs_inet.h>
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

MyRuLibApp::MyRuLibApp()
    :m_locale(NULL), m_collection(NULL)
{
}

MyRuLibApp::~MyRuLibApp()
{
    wxDELETE(m_locale);
	wxDELETE(m_collection);
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

	wxFileName filename = GetDatabaseFilename(config);
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

wxFileName MyRuLibApp::GetDatabaseFilename(FbDatabase &database)
{
	FbStandardPaths paths;
	wxFileName filename = paths.GetExecutablePath();
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

	wxString recent = database.GetText(FB_RECENT_0);
	if (!recent.IsEmpty()) {
		wxFileName filename = recent;
		if (filename.FileExists()) return filename;
	}

	FbDataOpenDlg dlg(NULL);
	if (dlg.ShowModal() == wxID_OK) {
		return dlg.GetFilename();
	}

	filename.SetPath(paths.GetUserConfigDir());
	return filename;
}

int MyRuLibApp::OnExit()
{
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
	int flags = WXSQLITE_OPEN_FULLMUTEX | WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE;

	try {
		FbMainDatabase dbMain;
		dbMain.Open(filename, wxEmptyString, flags);
		SetAppData(filename);
		FbParams params;
		params.LoadParams();
		params.AddRecent(filename, FbParams::GetText(DB_LIBRARY_TITLE));
		(new FbTextThread)->Execute();
		ZipReader::Init();
		wxDELETE(m_collection);
		m_collection = new FbCollection(filename);
	} catch (wxSQLite3Exception & e) {
		wxLogError(_("Database error: ") + e.GetMessage());
		return false;
	}
	return true;
}

const wxString MyRuLibApp::GetAppData()
{
	wxCriticalSectionLocker locker(m_section);
	return m_datafile;
};

const wxString MyRuLibApp::GetAppPath()
{
	wxCriticalSectionLocker locker(m_section);
	return wxFileName(m_datafile).GetPath();
};

void MyRuLibApp::SetAppData(const wxString &filename)
{
	wxCriticalSectionLocker locker(m_section);
	m_datafile = filename;
};

