#ifndef __FBDOWNLOADER_H__
#define __FBDOWNLOADER_H__

#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>

class FbInternetBook
{
	public:
		FbInternetBook(const wxString& md5sum);
		static wxString GetURL(const int id);
		bool Execute();
	private:
		bool DoDownload();
		bool DownloadUrl(const wxString &cookie = wxEmptyString);
		bool CheckZip();
		bool ReadFile(wxInputStream * in);
		void SaveFile(const bool success);
	private:
		int m_id;
		wxString m_url;
		wxString m_md5sum;
		wxString m_filetype;
		wxString m_filename;
		bool m_zipped;
};

class FbDownloader: public wxThread
{
	public:
		static wxString GetFilename(const wxString &md5sum, bool bCreateFolder = false);
		static void Start();
		static void Pause();
		static void Push(const wxString & md5sum);
		static void IsWaiting(const wxString & md5sum);
		bool IsRunning();
	protected:
		virtual void *Entry();
	private:
		void GetBooklist(wxArrayString &md5sum);
	private:
		static wxCriticalSection sm_queue;
		static bool sm_running;
		static wxArrayString sm_waitings;
};

#endif // __FBDOWNLOADER_H__
