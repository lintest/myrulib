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

