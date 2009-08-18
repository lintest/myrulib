#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>
#include "ImpContext.h"

class ParseThread : public wxThread
{
public:
    ParseThread(wxEvtHandler *frame);
    virtual void OnExit();
	static bool ParseXml(wxInputStream& stream, const wxString &name, int id_archive);
	void PostEvent(wxEvent& event);
    static int AddArchive(const wxString &filename);
private:
	static bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	static void AppendBook(ImportParsingContext &info, const wxString &name, const wxFileOffset size, int id_archive);
	static bool FindAnalog(wxInputStream& stream);
	static wxString ParseThread::CalcSHA1(wxInputStream& stream);
private:
    wxEvtHandler *m_frame;
};

class ImportThread : public ParseThread
{
public:
    ImportThread(wxEvtHandler *frame, const wxString &filename);
    virtual void *Entry();
private:
    wxString m_filename;
};

class FolderThread : public ParseThread
{
public:
    FolderThread(wxEvtHandler *frame, const wxString &dirname);
    virtual void *Entry();
    static bool ParseZip(const wxString &filename);
private:
    wxString m_dirname;
};

#endif // __FBTHREAD_H__
