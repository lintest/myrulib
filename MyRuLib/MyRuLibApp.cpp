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
#include "FbMainFrame.h"
#include "FbLogStream.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "ImpThread.h"

IMPLEMENT_APP(MyRuLibApp)

bool MyRuLibApp::OnInit()
{
	if(!ConnectToDatabase()) {
		wxLogFatalError(_("Error connecting to database!"));
		return false;
	}

	::wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxMemoryFSHandler);

	FbMainFrame * frame = new FbMainFrame;
	SetTopWindow(frame);
	frame->Show();

	ZipReader::Init();

	return true;
}

int MyRuLibApp::OnExit()
{
	return wxApp::OnExit();
}

bool MyRuLibApp::ConnectToDatabase()
{
	FbConfigDatabase dbConfig;
	dbConfig.Open();

	m_datafile = FbStandardPaths().GetDataFile();

	wxFileName logname = m_datafile;
	logname.SetExt(wxT("log"));
	wxLog *logger = new FbLogStream(logname.GetFullPath());
	wxLog::SetActiveTarget(logger);

	FbMainDatabase dbMain;
	dbMain.Open(m_datafile);

	FbParams().LoadParams();

	return true;
}

wxString FbStandardPaths::GetUserConfigDir() const
{
#if defined(__WIN32__)
	wxString result = wxStandardPaths::GetUserConfigDir();
#else
	wxString result = wxStandardPaths::GetUserConfigDir() + wxT("/.config/");
#endif

	if (!wxFileName::DirExists(result)) wxFileName::Mkdir(result);

	result = AppendAppName(result);
	if (!wxFileName::DirExists(result)) wxFileName::Mkdir(result);

	return result;
}

wxString FbStandardPaths::GetAppFileName() const
{
	if (wxGetApp().argc) {
		return wxString(wxGetApp().argv[0]);
	} else {
		return wxGetApp().GetAppName();
	}
}

wxString FbStandardPaths::GetDataFile() const
{
	wxFileName filename = GetDatabaseFilename();
	filename.Normalize();
	return filename.GetFullPath();
}

wxFileName FbStandardPaths::GetDatabaseFilename() const
{
	wxFileName filename = GetAppFileName();
	filename.SetExt(wxT("db"));

	if (wxGetApp().argc > 1) {
		wxString arg = wxGetApp().argv[1];
		if (wxFileName::DirExists(arg)) {
			wxFileName filename = wxGetApp().GetAppName() + wxT(".db");
			filename.SetPath(arg);
			return filename;
		}
		return wxFileName(arg);
	}

	if (filename.FileExists()) return filename;

	filename.SetPath(GetUserConfigDir());
	return filename;
}

wxString FbStandardPaths::GetConfigFile() const
{
	wxFileName filename = GetAppFileName();
	filename.SetExt(wxT("cfg"));
	filename.SetPath(GetUserConfigDir());
	filename.Normalize();
	return filename.GetFullPath();
}
