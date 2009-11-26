/***************************************************************
 * Name:	  MyRuLibApp.cpp
 * Purpose:   Code for Application Class
 * Author:	Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/app.h>
#include <wx/image.h>
#include <wx/fs_mem.h>
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbMainFrame.h"
#include "FbLogStream.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "FbDataOpenDlg.h"

IMPLEMENT_APP(MyRuLibApp)

bool MyRuLibApp::OnInit()
{
	FbConfigDatabase config;
	config.Open();

	OpenLog();

	wxFileName filename = GetDatabaseFilename(config);
	filename.Normalize();
	OpenDatabase(filename.GetFullPath());

	::wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxMemoryFSHandler);

	FbMainFrame * frame = new FbMainFrame;
	SetTopWindow(frame);
	frame->Show();

	return true;
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
		params.AddRecent(filename);
		ZipReader::Init();
	} catch (wxSQLite3Exception & e) {
		wxLogError(wxT("Database error: ") + e.GetMessage());
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
