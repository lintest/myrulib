#ifndef __FBIMPORTTHREAD_H__
#define __FBIMPORTTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "BaseThread.h"
#include "ImpContext.h"
#include "FbDatabase.h"

class FbImportBook: public ParsingContext
{
	public:
		FbImportBook(FbDatabase &database, const wxString &message, const wxString &md5sum = wxEmptyString);
		bool Load(wxInputStream& stream);
		void Save(const wxString &filename, wxFileOffset filesize, int id_archive = 0);
		static wxString CalcMd5(wxInputStream& stream);
	public:
		wxString title;
		wxString isbn;
		AuthorArray authors;
		SequenceArray sequences;
		wxString genres;
		AuthorItem * author;
		wxString text;
	private:
		int FindByMD5();
		int FindBySize(wxFileOffset filesize);
		void AppendBook(const wxString &filename, wxFileOffset size, int id_archive);
		void AppendFile(int id_book, const wxString &filename, int id_archive);
		void Convert();
	private:
		FbDatabase &m_database;
		wxString m_message;
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
