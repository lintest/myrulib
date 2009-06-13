/***************************************************************
 * Name:      MyRuLibApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/app.h>
#include <wx/image.h>
#include <DatabaseLayerException.h>
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "DBCreator.h"

IMPLEMENT_APP(MyRuLibApp)

bool MyRuLibApp::OnInit()
{
	if(!ConnectToDatabase()) {
		wxFAIL_MSG(_("Error connecting to database!"));
		return false;
	}

	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxJPEGHandler);

	MyRuLibMainFrame * frame = new MyRuLibMainFrame;
	SetTopWindow(frame);
	frame->Show();

	return true;
}

int MyRuLibApp::OnExit()
{
	wxDELETE(m_Database);
	return wxApp::OnExit();
}

wxString MyRuLibApp::GetAppPath()
{
    if (argc) {
        wxFileName app_filename = wxString(argv[0]);
        return app_filename.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    } else {
        return wxEmptyString;
    }
}

bool MyRuLibApp::ConnectToDatabase()
{
    wxFileName db_filename;
    if (wxGetApp().argc) {
        wxString app_filename = wxString(wxGetApp().argv[0]);
        db_filename = wxFileName(app_filename);
        db_filename.SetExt(wxT("db"));
    } else {
        db_filename = wxFileName(wxT("MyRuLib.db"));
    }

	wxString db_filepath = db_filename.GetFullPath();
	if (wxGetApp().argc>1)
		::wxRemoveFile(db_filepath);

	m_Database = new SqliteDatabaseLayer();
	bool bCreate = !wxFileExists(db_filepath);
	try	{
		m_Database->Open(db_filepath);
		if(bCreate)	{
			DBCreator(m_Database).CreateDatabase();
		}
	}
	catch(DatabaseLayerException & e) {
		wxFAIL_MSG(e.GetErrorMessage());
		return false;
	}
	return true;
}
