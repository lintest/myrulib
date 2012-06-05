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
	FbZipInputStream(const wxString & archname, const wxString & filename, bool info);
	FbZipInputStream(const wxString & archname, const wxString & filename);
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

	const wxString & GetFileType() const
		{ return m_filetype; }

	void ShowError() const;

	static void ShellExecute(const wxString &filename);

	static void ShellExecute(const wxString &archname, const wxString &filename);

private:
	wxString GetFileName() const;
	wxString CreateDataFile(const wxString &tempfile) const;
	wxString CreateTempFile(const wxString &tempfile) const;
	wxString GetError(const wxString &name, const wxString &path = wxEmptyString);
	void DoDownload() const;
	bool WrongExt() const;

private:
	const int m_id;
	wxString m_md5sum;
	wxString m_filename;
	wxString m_filetype;
	wxString m_message;
	wxInputStream * m_stream;
};

#endif // __ZIPREADER_H__
