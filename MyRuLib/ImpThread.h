#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>
#include <DatabaseLayer.h>
#include "BaseThread.h"
#include "ImpContext.h"

class ImportThread : public BaseThread
{
public:
    ImportThread();
    virtual ~ImportThread();
    virtual void OnExit();
	bool ParseXml(wxInputStream& stream, const wxString &name, int id_archive);
    int AddArchive(const wxString &filename, const int file_size, const int file_count);
private:
	bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	void AppendBook(ImportParsingContext &info, const wxString &filename, const wxFileOffset size, const int id_archive);
	void AppendFile(const int id_book, const int id_archive, const wxString &new_name);
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
        psLastMember,
    };
    PreparedStatement * m_statements[psLastMember];
    PreparedStatement * GetPreparedStatement(PSItem psItem);
    wxString GetSQL(PSItem psItem);
    DatabaseLayer * m_database;
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
    bool ParseZip(const wxString &filename);
    void DoStep(const wxString &msg) { ImportThread::DoStep(msg); };
private:
    wxString m_dirname;
};

#endif // __FBTHREAD_H__
