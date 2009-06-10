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

	m_Database = new SqliteDatabaseLayer();
	bool bCreate = !wxFileExists(db_filename.GetFullPath());
	try	{
		m_Database->Open(db_filename.GetFullPath());
		if(bCreate)	{
			DBCreator creator(m_Database);
			creator.CreateDatabase();
		}
	}
	catch(DatabaseLayerException & e) {
		wxFAIL_MSG(e.GetErrorMessage());
		return false;
	}
	return true;
}
