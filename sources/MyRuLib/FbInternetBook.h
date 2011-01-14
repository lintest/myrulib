#ifndef __FBINTERNETBOOK_H__
#define __FBINTERNETBOOK_H__

#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include "FbDownloader.h"

class FbInternetBook
{
	public:
		FbInternetBook(FbDownloader * owner, const wxString& md5sum);
		static wxString GetURL(const int id, const wxString& md5sum = wxEmptyString);
		bool Execute();
	private:
		bool DoDownload();
		bool DownloadUrl(const wxString &cookie = wxEmptyString);
		bool CheckZip();
		bool ReadFile(wxInputStream * in);
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
