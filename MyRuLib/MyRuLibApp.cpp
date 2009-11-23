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
#include "ImpThread.h"

IMPLEMENT_APP(MyRuLibApp)

bool MyRuLibApp::OnInit()
{
	OpenConfig();

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
	m_datafile = FbStandardPaths().GetDataFile();

	FbMainDatabase dbMain;
	dbMain.Open(m_datafile);
	FbParams().LoadParams();

	return true;
}

bool MyRuLibApp::OpenConfig()
{
	FbConfigDatabase dbConfig;
	dbConfig.Open();

	wxFileName logname = FbDatabase::GetConfigName();
	logname.SetExt(wxT("log"));
	wxLog * logger = new FbLogStream(logname.GetFullPath());
	wxLog::SetActiveTarget(logger);

	return true;
}

bool MyRuLibApp::OpenDatabase(const wxString &filename, bool bCreateNew)
{
	int flags = WXSQLITE_OPEN_FULLMUTEX | WXSQLITE_OPEN_READWRITE;
	if (bCreateNew) flags |= WXSQLITE_OPEN_CREATE;

	if (bCreateNew) wxRemoveFile(filename);

	try {
		FbMainDatabase dbMain;
		dbMain.Open(filename, wxEmptyString, flags);
		FbParams().LoadParams();
		wxCriticalSectionLocker locker(m_section);
		m_datafile = filename;
	} catch (wxSQLite3Exception & e) {
		wxLogError(wxT("Database open error: ") + e.GetMessage());
		return false;
	}
	return true;
}

wxString MyRuLibApp::GetAppData()
{
	wxCriticalSectionLocker locker(m_section);
	return m_datafile;
};

wxString MyRuLibApp::GetAppPath()
{
	wxCriticalSectionLocker locker(m_section);
	return wxFileName(m_datafile).GetPath();
};
