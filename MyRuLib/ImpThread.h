#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>
#include "ImpContext.h"

class ImportThread : public wxThread
{
public:
    ImportThread(wxEvtHandler *frame);
    virtual void OnExit();
	static bool ParseXml(wxInputStream& stream, const wxString &name, int id_archive);
	void PostEvent(wxEvent& event);
    static int AddArchive(const wxString &filename);
    wxString m_info;
private:
	static bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	static void AppendBook(ImportParsingContext &info, const wxString &name, const wxFileOffset size, int id_archive);
	static bool FindAnalog(wxString sha1sum);
	static wxString CalcSHA1(wxInputStream& stream);
private:
    wxEvtHandler *m_frame;
};

class ZipImportThread : public ImportThread
{
public:
    ZipImportThread(wxEvtHandler *frame, const wxString &filename);
    virtual void *Entry();
private:
    wxString m_filename;
};

class DirImportThread : public ImportThread
{
public:
    DirImportThread(wxEvtHandler *frame, const wxString &dirname);
    virtual void *Entry();
    static bool ParseZip(const wxString &filename);
private:
    wxString m_dirname;
};

#endif // __FBTHREAD_H__
