#ifndef __ZIPREADER_H__
#define __ZIPREADER_H__

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

class ZipReader
{
private:
    wxFFileInputStream *m_file;
    wxZipInputStream *m_zip;
    bool m_zipOk;
    bool m_fileOk;
    int m_id;
    wxString m_file_name;
    wxString m_zip_name;
private:
    bool FindZip(wxFileName &zip_name);
    bool FindEntry(const wxString &file_name);
public:
	ZipReader(int id);
	virtual ~ZipReader();
	bool IsOK() {return m_zipOk && m_fileOk;};
	void ShowError();
    wxString GetErrorText();
	wxZipInputStream & GetZip() {return *m_zip;};
};

#endif // __ZIPREADER_H__
