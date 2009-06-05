/***************************************************************
 * Name:      MyRuLibApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/image.h>
#include <DatabaseLayerException.h>
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "DBCreator.h"

IMPLEMENT_APP(MyRuLibApp)

bool MyRuLibApp::OnInit() {

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

int MyRuLibApp::OnExit() {
	wxDELETE(m_Authors);
	wxDELETE(m_Books);
	wxDELETE(m_Params);
	if(m_Database) {
		if(m_Database->IsOpen())
		{
			m_Database->Close();
		}
		wxDELETE(m_Database);
	}
	return wxApp::OnExit();
}



bool MyRuLibApp::ConnectToDatabase()
{
	m_Database = new SqliteDatabaseLayer();
	wxString db_filename(wxT("MyRuLib.db"));
	bool bCreate = !wxFileExists(db_filename);
	try	{
		m_Database->Open(db_filename);
		if(bCreate)	{
			DBCreator creator(m_Database);
			creator.CreateDatabase();
		}
	}
	catch(DatabaseLayerException & e) {
		wxFAIL_MSG(e.GetErrorMessage());
		return false;
	}
	try	{
		m_Authors = new Authors(wxGetApp().GetDatabase(), wxT("authors"));
		m_Books = new Books(wxGetApp().GetDatabase(), wxT("books"));
		m_Params = new Params(wxGetApp().GetDatabase(), wxT("params"));
	}
	catch(DatabaseLayerException & e) {
		wxActiveRecord::ProcessException(e);
		return false;
	}
	return true;
}

DatabaseLayer * MyRuLibApp::GetDatabase() {
	return m_Database;
}

Authors * MyRuLibApp::GetAuthors() {
	return m_Authors;
}

Books * MyRuLibApp::GetBooks() {
	return m_Books;
}

Params * MyRuLibApp::GetParams() {
	return m_Params;
}
