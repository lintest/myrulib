#ifndef __IMPTHREAD_H__
#define __IMPTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "BaseThread.h"
#include "ImpContext.h"
#include "FbDatabase.h"

class ImportThread : public BaseThread
{
public:
	ImportThread();
	virtual void OnExit();
protected:
	bool ParseXml(wxInputStream& stream, const wxString &filename, const int id_archive = 0, const wxString &md5sum = wxEmptyString);
	static wxString ParseMd5(wxInputStream& stream);
	int AppendZip(const wxString &filename, const int size, const int count);
	wxString GetRelative(const wxString &filename);
protected:
	FbCommonDatabase m_database;
	wxString m_basepath;
private:
	bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	void AppendBook(ImportParsingContext &info, const wxString &filename, const wxFileOffset size, const int id_archive);
	void AppendFile(const int id_book, const int id_archive, const wxString &new_name);
	int FindByMD5(const wxString &sha1sum);
	int FindBySize(const wxString &sha1sum, wxFileOffset size);
};

class ZipImportThread : public ImportThread
{
public:
	ZipImportThread(const wxArrayString &filelist): ImportThread(), m_filelist(filelist) {};
	virtual void *Entry();
private:
	void ImportFile(const wxString & zipname);
	const wxArrayString m_filelist;
};

class DirImportThread : public ImportThread
{
public:
	DirImportThread(const wxString &dirname): m_dirname(dirname) {};
	virtual void *Entry();
	bool ParseZip(const wxString &zipname);
	bool ParseXml(const wxString &filename);
	void DoStep(const wxString &msg) { ImportThread::DoStep(msg); };
private:
	wxString m_dirname;
};

#endif // __IMPTHREAD_H__
