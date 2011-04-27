#ifndef __FBIMPORTTHREAD_H__
#define __FBIMPORTTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include <wx/zipstrm.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "FbCounter.h"
#include "FbDatabase.h"

class FbImportThread
	: public FbProgressThread
{
public:
	FbImportThread(wxEvtHandler * owner, wxThreadKind kind = wxTHREAD_DETACHED);
	bool OnFile(const wxString &filename, bool progress);
	void SetRoot(const wxString & dir);
protected:
	virtual void * Entry();
	virtual void DoParse() = 0;
	wxString GetRelative(const wxString &filename);
	wxString GetAbsolute(const wxString &filename);
protected:
	FbDatabase * m_database;
	wxString m_basepath;
	bool m_fullpath;
	friend class FbImportZip;
	friend class FbImportBook;
};

class FbZipImportThread
	: public FbImportThread
{
public:
	FbZipImportThread(wxEvtHandler * owner, const wxArrayString &filelist, wxThreadKind kind = wxTHREAD_DETACHED)
		: FbImportThread(owner, kind), m_filelist(filelist) {};
	virtual void DoParse();
private:
	const wxArrayString m_filelist;
};

class FbDirImportThread
	: public FbImportThread
{
public:
	FbDirImportThread(wxEvtHandler * owner, const wxString &dirname, wxThreadKind kind = wxTHREAD_DETACHED)
		: FbImportThread(owner, kind), m_dirname(dirname) {};
	virtual void DoParse();
private:
	wxString m_dirname;
	friend class FbImportTraverser;
};

class FbLibImportThread
	: public FbDirImportThread
{
public:
	FbLibImportThread(wxEvtHandler * owner, const wxString &file, const wxString &dir, const wxString &lib, bool import);
protected:
	virtual void * Entry();
private:
	bool Execute();
	bool CreateLib();
	bool SaveTo(wxInputStream &in, const wxString &filename, const wxString &msg);
	bool Download(const wxString &filename);
	bool Extract(const wxString &filename);
private:
	wxString m_file;
	wxString m_dir;
	wxString m_lib;
	bool m_import;
};

#endif // __FBIMPORTTHREAD_H__
