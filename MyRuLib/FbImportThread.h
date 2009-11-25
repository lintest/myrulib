#ifndef __FBIMPORTTHREAD_H__
#define __FBIMPORTTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "BaseThread.h"
#include "ImpContext.h"
#include "FbDatabase.h"

class FbImportBook: public ImportParsingContext
{
	public:
		FbImportBook(FbDatabase &database, const wxString &filename, const wxFileOffset filesize, const wxString &md5sum = wxEmptyString);
		bool Load(wxInputStream& stream);
		void Save(const wxString &filename, int archive = 0);
		static wxString CalcMd5(wxInputStream& stream);
	private:
		int FindByMD5();
		int FindBySize();
		void AppendBook();
		void AppendFile();
		void Convert();
	private:
		FbDatabase &m_database;
		wxFileOffset m_filesize;
		wxString m_md5sum;
};

class FbImportThread : public BaseThread
{
public:
	FbImportThread();
	virtual void OnExit();
protected:
	bool ParseXml(wxInputStream& stream, const wxString &filename, const int id_archive = 0, const wxString &md5sum = wxEmptyString);
	wxString ParseMd5(wxInputStream& stream);
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
	bool ParseZip(const wxString &zipname);
	bool ParseXml(const wxString &filename);
	void DoStep(const wxString &msg) { FbImportThread::DoStep(msg); };
private:
	wxString m_dirname;
};

#endif // __FBIMPORTTHREAD_H__
