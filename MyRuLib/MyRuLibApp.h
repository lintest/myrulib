#ifndef __MYRULIBAPP_H__
#define __MYRULIBAPP_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include "FbDatabase.h"

class MyRuLibApp : public wxApp
{
public:
    MyRuLibApp();
    virtual ~MyRuLibApp();
	virtual bool OnInit();
	virtual int OnExit();
	bool OpenDatabase(const wxString &filename);
	const wxString GetAppData();
	const wxString GetAppPath();
	void Localize(int language = wxLANGUAGE_DEFAULT);
	int GetLanguage();
private:
	void OpenLog();
	wxFileName GetDatabaseFilename(FbDatabase &database);
	void SetAppData(const wxString &filename);
	void LoadBlankImage();
private:
	wxCriticalSection m_section;
	wxString m_datafile;
	wxLocale * m_locale;
};

DECLARE_APP(MyRuLibApp)

#endif // __MYRULIBAPP_H__
