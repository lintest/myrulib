#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>
#include "ImpContext.h"

class ImportThread : public wxThread
{
public:
    ImportThread(wxEvtHandler *frame, const wxString &filename);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();
	static bool ParseXml(wxInputStream& stream, const wxString &name, const wxFileOffset size, int id_archive);
    int AddArchive();
private:
	static bool LoadXml(wxInputStream& stream, ImportParsingContext &ctx);
	static void AppendBook(ImportParsingContext &info, const wxString &name, const wxFileOffset size, int id_archive);
private:
    unsigned m_count;
    wxString m_filename;
    wxEvtHandler *m_frame;
};

#endif // __FBTHREAD_H__
