#ifndef __FBIMPORTTHREAD_H__
#define __FBIMPORTTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include <wx/zipstrm.h>
#include "BaseThread.h"
#include "FbDatabase.h"
#include "FbCounter.h"

class FbImportThread : public BaseThread
{
public:
	FbImportThread();
	virtual void OnExit();
protected:
	bool ParseXml(wxInputStream& stream, const wxString &filename, const int id_archive = 0, const wxString &md5sum = wxEmptyString);
	wxString ParseMd5(wxInputStream& stream);
	wxString GetRelative(const wxString &filename);
	wxString GetAbsolute(const wxString &filename);
protected:
	FbCommonDatabase m_database;
	FbCounter m_counter;
	wxString m_basepath;
	bool m_fullpath;
	friend class FbImpotrZip;
	friend class FbImportBook;
};

WX_DECLARE_STRING_HASH_MAP(wxZipEntry*, FbZipEntryMap);

WX_DECLARE_OBJARRAY(wxZipEntry*, FbZipEntryList);

class FbImpotrZip
{
	public:
		FbImpotrZip(FbImportThread *owner, wxInputStream &in, const wxString &zipname);
		int Save();
	public:
		bool IsOk() { return m_ok; };
		void Make(FbImportThread *owner = NULL);
	private:
		bool OpenEntry(wxZipEntry &entry) { return m_zip.OpenEntry(entry); };
		wxZipEntry * GetInfo(const wxString & filename);
	private:
		FbDatabase &m_database;
		FbZipEntryList m_list;
		FbZipEntryMap m_map;
		wxCSConv m_conv;
		wxZipInputStream m_zip;
		wxString m_filename;
		wxString m_filepath;
		wxFileOffset m_filesize;
		friend class FbImportBook;
		bool m_ok;
		int m_id;
};

class FbZipImportThread : public FbImportThread
{
public:
	FbZipImportThread(const wxArrayString &filelist): FbImportThread(), m_filelist(filelist) {};
	virtual void *Entry();
private:
	void ImportFile(const wxString & zipname);
	const wxArrayString m_filelist;
};

class FbDirImportThread : public FbImportThread
{
public:
	FbDirImportThread(const wxString &dirname): m_dirname(dirname) {};
	virtual void *Entry();
	void ParseZip(const wxString &zipname);
	void ParseXml(const wxString &filename);
	void DoStep(const wxString &msg) { FbImportThread::DoStep(msg); };
private:
	wxString m_dirname;
};

#endif // __FBIMPORTTHREAD_H__
