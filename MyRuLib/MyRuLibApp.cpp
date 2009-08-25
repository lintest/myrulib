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
#include <wx/fs_mem.h>
#include <wx/stdpaths.h>
#include <DatabaseLayerException.h>
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "DBCreator.h"
#include "FbParams.h"
#include "ZipReader.h"

IMPLEMENT_APP(MyRuLibApp)

bool MyRuLibApp::OnInit()
{
	if(!ConnectToDatabase()) {
		wxFAIL_MSG(_("Error connecting to database!"));
		return false;
	}

	::wxInitAllImageHandlers();

    wxFileSystem::AddHandler(new wxMemoryFSHandler);

	MyRuLibMainFrame * frame = new MyRuLibMainFrame;
	SetTopWindow(frame);
	frame->Show();

	ZipReader::Init();

	return true;
}

int MyRuLibApp::OnExit()
{
	wxDELETE(m_Database);
	return wxApp::OnExit();
}

class MyStandardPaths: public wxStandardPaths
{
	public:
		virtual wxString GetDataFile() const;
		virtual wxString MyStandardPaths::GetAppFileName() const;
	protected:
		virtual wxString GetDataDir() const;
	private:
		wxFileName GetDatabaseFilename() const;
};

wxString MyStandardPaths::GetDataDir() const
{
#if defined(__WIN32__)
	return wxStandardPaths::GetUserConfigDir();
#else
	return wxStandardPaths::GetUserConfigDir() + wxT("/.local/");
#endif
}

wxString MyStandardPaths::GetAppFileName() const
{
    if (wxGetApp().argc) {
        return wxString(wxGetApp().argv[0]);
    } else {
        return wxGetApp().GetAppName();
    }
}

wxString MyStandardPaths::GetDataFile() const
{
	wxFileName filename = GetDatabaseFilename();
	filename.Normalize();
	return filename.GetFullPath();
}

wxFileName MyStandardPaths::GetDatabaseFilename() const
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

	if (filename.FileExists())
		return filename;

	wxString filepath = GetDataDir();
	if (!wxFileName::DirExists(filepath))
		wxFileName::Mkdir(filepath);

	filepath = AppendAppName(filepath);
	if (!wxFileName::DirExists(filepath))
		wxFileName::Mkdir(filepath);

	filename.SetPath(filepath);
	return filename;
}

wxString MyRuLibApp::GetAppPath() const
{
	wxFileName filename = m_datafile;
	return filename.GetPath();
}

bool MyRuLibApp::ConnectToDatabase()
{
	m_datafile = MyStandardPaths().GetDataFile();

	m_Database = new SqliteDatabaseLayer();
	bool bCreate = !wxFileExists(m_datafile);
	try	{
		m_Database->Open(m_datafile);
		DBCreator creator(m_Database);
		if(bCreate)	creator.CreateDatabase(m_datafile);
		creator.UpgradeDatabase();
	} catch(DatabaseLayerException & e) {
		wxMessageBox(e.GetErrorMessage());
		wxFAIL_MSG(e.GetErrorMessage());
		return false;
	}
	return true;
}
