#ifndef __ZIPREADER_H__
#define __ZIPREADER_H__

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/arrimpl.cpp>
#include "FbDatabase.h"

class ZipReader
{
public:
	ZipReader(int id, bool bShowError = true, bool bInfoOnly = false);
	virtual ~ZipReader();
	static void Init();
	bool IsOK() {return m_zipOk && m_fileOk;};
	void ShowError();
	wxString GetErrorText() {return m_info;};
	wxInputStream & GetZip() {return *m_result;};
private:
	wxString GetInfoName(const wxString &filename);
	bool FindEntry(const wxString &file_name);
	void OpenZip(const wxString &zipname, const wxString &filename);
	void OpenFile(const wxString &filename);
	void OpenDownload(FbDatabase &database);
private:
	wxCSConv conv;
	wxFFileInputStream *m_file;
	wxZipInputStream *m_zip;
	wxInputStream *m_result;
	bool m_zipOk;
	bool m_fileOk;
	int m_id;
	wxString m_info;
};

#endif // __ZIPREADER_H__
