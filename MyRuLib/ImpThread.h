#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "BaseThread.h"
#include "ImpContext.h"

class ImportThread : public BaseThread
{
public:
    ImportThread();
    virtual void OnExit();
	bool ParseXml(wxInputStream& stream, const wxString &name, const wxString &path, const int id_archive);
    int AddArchive(const wxString &name, const wxString &path, const int size, const int count);
private:
	bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	void AppendBook(ImportParsingContext &info, const wxString &name, const wxString &path, const wxFileOffset size, const int id_archive);
	void AppendFile(const int id_book, const int id_archive, const wxString &new_name, const wxString &new_path);
	int FindBySHA1(const wxString &sha1sum);
	int FindBySize(const wxString &sha1sum, wxFileOffset size);
private:
    enum PSItem {
        psFindBySize = 0,
        psFindBySha1,
        psUpdateSha1,
        psSearchFile,
        psAppendFile,
        psSearchArch,
        psAppendArch,
        psAppendBook,
        psAppendSeqs,
        psLastMember,
    };
    wxSQLite3Statement GetPreparedStatement(PSItem psItem);
    wxString GetSQL(PSItem psItem);
    wxSQLite3Database & m_database;
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
    DirImportThread(const wxString &dirname);
    virtual void *Entry();
    bool ParseZip(const wxString &zipname);
    bool ParseXml(const wxString &filename);
    void DoStep(const wxString &msg) { ImportThread::DoStep(msg); };
private:
    static wxString Normalize(const wxString &filename);
    wxString Relative(const wxString &filename);
private:
    wxString m_dirname;
    int m_position;
};

class BooksCountThread : public BaseThread
{
public:
    static void Execute();
    BooksCountThread(): BaseThread() {};
    virtual void *Entry();
};

#endif // __FBTHREAD_H__
