#ifndef __FBINTERNETBOOK_H__
#define __FBINTERNETBOOK_H__

#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include "FbDownloader.h"

class FbInternetBook
{
	public:
		static wxString GetURL(const int id, const wxString& md5sum = wxEmptyString);
		static bool Download(wxEvtHandler * owner, const wxString & address, const wxString & filename, const wxString &cookie = wxEmptyString);
		static bool Download(const wxString & address, wxString & filename, const wxString &cookie = wxEmptyString);
		FbInternetBook(FbDownloader * owner, const wxString& md5sum);
		bool Execute();
	private:
		bool DoDownload();
		bool DownloadUrl(const wxString &cookie = wxEmptyString);
		bool CheckFile();
		bool CheckZip();
		void SaveFile(const bool success);
	private:
		int m_id;
		FbDownloader * m_owner;
		wxString m_url;
		wxString m_md5sum;
		wxString m_filetype;
		wxString m_filename;
		bool m_zipped;
};

#endif // __FBINTERNETBOOK_H__
