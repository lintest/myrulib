#ifndef __ZIPREADER_H__
#define __ZIPREADER_H__

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include "FbDatabase.h"
#include "FbSmartPtr.h"

class FbTempEraser
{
public:
	static void Add(const wxString &filename);
	static bool sm_erase;
private:
	FbTempEraser() {}
	virtual ~FbTempEraser();
	wxStringList m_filelist;
};

class FbZipInputStream : public wxZipInputStream
{
public:
	FbZipInputStream(const wxString & archname, const wxString & filename = wxEmptyString);
	FbZipInputStream(const wxString & archname, bool info);
	FbZipInputStream(wxInputStream * stream, bool info);
	virtual wxFileOffset SeekI(wxFileOffset pos, wxSeekMode mode = wxFromStart);
	virtual bool IsOk() const { return m_ok && &m_entry && wxZipInputStream::IsOk(); }
private:
	FbSmartPtr<wxZipEntry> m_entry;
	bool m_ok;
};

class FbFileReader : public wxObject
{
public:
	FbFileReader(int book, bool info = false);

	virtual ~FbFileReader();

	void Open() const;

	wxInputStream & GetStream() const
		{ return * m_stream; }

	bool IsOk() const
		{ return m_stream && m_stream->IsOk(); }

	const wxString & GetFileName() const
		{ return m_filename; }

	const wxString & GetFileType() const
		{ return m_filetype; }

	void ShowError() const {}

private:
	wxString GetError(const wxString &name, const wxString &path = wxEmptyString);
	void DoDownload() const;

private:
	const int m_id;
	wxString m_md5sum;
	wxString m_filename;
	wxString m_filetype;
	wxString m_message;
	wxInputStream * m_stream;
};

class ZipReader
{
public:
	ZipReader(int id, bool bShowError = true, bool bInfoOnly = false);
	virtual ~ZipReader();
	bool IsOk() { return m_zipOk && m_fileOk; }
	void ShowError();
	wxString GetErrorText() {return m_info;};
	wxInputStream & GetZip() {return *m_result;};
	wxString GetMd5() {return m_md5sum;};
private:
	bool FindEntry(const wxString &file_name);
	void OpenZip(const wxString &archname, const wxString &filename);
	void OpenFile(const wxString &filename);
	void OpenDownload(FbDatabase &database, bool bInfoOnly);
	bool OpenEntry(bool bInfoOnly);
private:
	wxCSConv conv;
	wxFFileInputStream *m_file;
	wxZipInputStream *m_zip;
	wxInputStream *m_result;
	bool m_zipOk;
	bool m_fileOk;
	int m_id;
	wxString m_info;
	wxString m_md5sum;
};

#endif // __ZIPREADER_H__
