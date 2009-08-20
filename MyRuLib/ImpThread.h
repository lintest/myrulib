#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>
#include "BaseThread.h"
#include "ImpContext.h"

class ImportThread : public BaseThread
{
public:
    virtual void OnExit();
	static bool ParseXml(wxInputStream& stream, const wxString &name, int id_archive);
    static int AddArchive(const wxString &filename);
private:
	static bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	static void AppendBook(ImportParsingContext &info, const wxString &name, const wxFileOffset size, int id_archive);
	static bool FindBySHA1(const wxString &sha1sum);
	static bool FindBySize(const wxString &sha1sum, wxFileOffset size);
};

class ZipImportThread : public ImportThread
{
public:
    ZipImportThread(const wxString &filename): ImportThread(), m_filename(filename) {};
    virtual void *Entry();
private:
    wxString m_filename;
};

class DirImportThread : public ImportThread
{
public:
    DirImportThread(const wxString &dirname): m_dirname(dirname) {};
    virtual void *Entry();
    static bool ParseZip(const wxString &filename);
    void DoStep(const wxString &msg) { ImportThread::DoStep(msg); };
private:
    wxString m_dirname;
};

#endif // __FBTHREAD_H__
