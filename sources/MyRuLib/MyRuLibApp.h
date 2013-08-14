#ifndef __MYRULIBAPP_H__
#define __MYRULIBAPP_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include "FbDatabase.h"
#include "FbDownloader.h"
#include "FbBookEvent.h"

class FbLocale;

class FbCollection;

class MyRuLibApp : public wxApp
{
	public:
		MyRuLibApp();
		virtual bool OnInit();
		virtual int OnExit();
		bool OpenDatabase(const wxString &filename);
		const wxString GetLibFile() const;
		const wxString GetLibPath() const;
		const wxString GetAppFile() const;
		const wxString GetAppPath() const;
		void Localize();
		void StartDownload();
		void StopDownload();
		void UpdateLibPath();
		FbCollection * GetCollection();
	protected:
		#ifdef __WXMSW__
		virtual int FilterEvent(wxEvent & event);
		#endif // __WXMSW__
		virtual void OnUnhandledException() {}
		virtual bool OnExceptionInMainLoop() { return true; }
	private:
		void SetLibFile(const wxString & filename);
		wxFileName GetDatabaseFile();
		void SetAppData(const wxString &filename);
		void LoadBlankImage();
	private:
		static wxCriticalSection sm_section;
		wxString m_LibFile;
		wxString m_LibPath;
		FbLocale * m_locale;
		FbCollection * m_collection;
		FbDownloader * m_downloader;
	private:
		void OnImageEvent(FbImageEvent & event);
		DECLARE_EVENT_TABLE()
};

DECLARE_APP(MyRuLibApp)

#endif // __MYRULIBAPP_H__
