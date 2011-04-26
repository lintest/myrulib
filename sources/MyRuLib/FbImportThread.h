#ifndef __FBIMPORTTHREAD_H__
#define __FBIMPORTTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include <wx/zipstrm.h>
#include "FbThread.h"
#include "FbDatabase.h"
#include "FbCounter.h"

class FbImportThread 
	: public FbProgressThread
{
public:
	FbImportThread();
	virtual void * Entry();
	virtual void OnExit();
	bool OnFile(const wxString &filename, bool progress);
protected:
	virtual void DoParse() = 0;
	wxString GetRelative(const wxString &filename);
	wxString GetAbsolute(const wxString &filename);
protected:
	FbCommonDatabase m_database;
	FbCounter m_counter;
	wxString m_basepath;
	bool m_fullpath;
	friend class FbImportZip;
	friend class FbImportBook;
};

class FbZipImportThread
	: public FbImportThread
{
public:
	FbZipImportThread(const wxArrayString &filelist): FbImportThread(), m_filelist(filelist) {};
	virtual void DoParse();
private:
	const wxArrayString m_filelist;
};

class FbDirImportThread
	: public FbImportThread
{
public:
	FbDirImportThread(const wxString &dirname): m_dirname(dirname) {};
	virtual void DoParse();
private:
	wxString m_dirname;
	friend class FbImportTraverser;
};

#endif // __FBIMPORTTHREAD_H__
