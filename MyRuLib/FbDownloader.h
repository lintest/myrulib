#ifndef __FBDOWNLOADER_H__
#define __FBDOWNLOADER_H__

#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>

class FbDownloader: public wxThread
{
	public:
		static wxString GetFilename(const wxString &md5sum, bool bCreateFolder = false);
		static wxString GetURL(const int id);
		static void Start();
		static void Pause();
		bool IsRunning();
	protected:
		virtual void *Entry();
	private:
		void GetBooklist(wxArrayString &md5sum);
	private:
		static wxCriticalSection sm_queue;
		static bool sm_running;

};

#endif // __FBDOWNLOADER_H__
