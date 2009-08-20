#ifndef __ZIPREADER_H__
#define __ZIPREADER_H__

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

class ZipReader
{
public:
	ZipReader(int id);
	virtual ~ZipReader();
	static void Init();
	bool IsOK() {return m_zipOk && m_fileOk;};
	void ShowError();
    wxString GetErrorText();
	wxInputStream & GetZip() {return *m_result;};
private:
    bool FindZip(wxFileName &zip_name, wxString &path);
    bool FindEntry(const wxString &file_name);
    void OpenZip(const wxString &zipname);
    void OpenFile(const wxString &filename);
private:
    wxFFileInputStream *m_file;
    wxZipInputStream *m_zip;
    wxInputStream *m_result;
    bool m_zipOk;
    bool m_fileOk;
    int m_id;
    wxString m_file_name;
    wxString m_zip_name;
};

#endif // __ZIPREADER_H__
