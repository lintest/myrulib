#ifndef __MYRULIBAPP_H__
#define __MYRULIBAPP_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include "FbDatabase.h"
#include "FbDownloader.h"
#include "FbBookEvent.h"

#define MAX_IMAGE_WIDTH 200

class FbLocale;

class FbCollection;

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
		const wxString GetLibPath();
		void Localize();
		FbCollection * GetCollection() { return m_collection; }
		void StartDownload();
		void StopDownload();
	protected:
		virtual void OnUnhandledException() {}
		virtual bool OnExceptionInMainLoop() { return false; }
	private:
		void OpenLog();
		wxFileName GetDatabaseFilename(FbDatabase &database);
		void SetAppData(const wxString &filename);
		void LoadBlankImage();
	private:
		wxCriticalSection m_section;
		wxString m_datafile;
		FbLocale * m_locale;
		FbCollection * m_collection;
		FbDownloader * m_downloader;
	private:
		void OnImageEvent(FbImageEvent & event);
		DECLARE_EVENT_TABLE()
};

DECLARE_APP(MyRuLibApp)

#endif // __MYRULIBAPP_H__
