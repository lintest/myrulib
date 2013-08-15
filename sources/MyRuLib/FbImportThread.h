#ifndef __FBIMPORTTHREAD_H__
#define __FBIMPORTTHREAD_H__

#include <wx/wx.h>
#include "wx/FbSQLite3.h"
#include <wx/zipstrm.h>
#include "wx/FbSQLite3.h"
#include "FbThread.h"
#include "FbDatabase.h"

#define fbIMP_IMPORT   0x01
#define fbIMP_ONLY_NEW 0x02
#define fbIMP_ONLY_MD5 0x04

class FbImportThread
	: public FbProgressThread
{
public:
	FbImportThread(wxEvtHandler * owner, long flags);
	bool OnFile(const wxString &filename, bool progress, bool only_new);
	void SetRoot(const wxString & dir);
	FbDatabase * GetDatabase() { return m_database; }
	wxString GetRelative(const wxString &filename);
	wxString GetAbsolute(const wxString &filename);
protected:
	virtual void * Entry();
	virtual bool Execute();
	virtual void DoParse(bool only_new) = 0;
    bool HasFlag(long flag) const { return (m_flags & flag) != 0; }
protected:
	FbDatabase * m_database;
	wxString m_basepath;
	wxString m_volume;
	bool m_fullpath;
	const long m_flags;
};

class FbZipImportThread
	: public FbImportThread
{
public:
	FbZipImportThread(wxEvtHandler * owner, const wxArrayString &filelist, long flags = fbIMP_IMPORT)
		: FbImportThread(owner, flags), m_filelist(filelist) {};
	virtual void DoParse(bool only_new);
private:
	const wxArrayString m_filelist;
};

class FbDirImportThread
	: public FbImportThread
{
public:
	FbDirImportThread(wxEvtHandler * owner, const wxString &dirname, long flags = fbIMP_IMPORT)
		: FbImportThread(owner, flags), m_dirname(dirname) {};
	virtual void DoParse(bool only_new);
private:
	wxString m_dirname;
	friend class FbImportTraverser;
};

class FbLibImportThread
	: public FbDirImportThread
{
public:
	FbLibImportThread(wxEvtHandler * owner, const wxString &file, const wxString &dir, const wxString &lib, long flags);
protected:
	virtual bool Execute();
private:
	bool CreateLib();
	bool SaveTo(wxInputStream &in, const wxString &filename, const wxString &msg);
	bool Download(const wxString &filename);
	bool Extract(const wxString &filename);
private:
	const wxString m_file;
	const wxString m_dir;
	const wxString m_lib;
};

#endif // __FBIMPORTTHREAD_H__
