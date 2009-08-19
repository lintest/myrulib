#ifndef __EXPTHREAD_H__
#define __EXPTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>

class ExportFileItem {
public:
    ExportFileItem(const wxFileName &n, const int i): filename(n), id(i) {};
public:
    wxFileName filename;
    int id;
};

WX_DECLARE_OBJARRAY(ExportFileItem, ExportFileArray);

class ExportThread : public wxThread
{
public:
    ExportThread(wxEvtHandler *frame, bool compress): wxThread(), m_frame(frame), m_compress(compress) {};
    wxString m_info;
    virtual void *Entry();
public:
    ExportFileArray m_filelist;
private:
    void WriteFileItem(ExportFileItem &item);
private:
    wxEvtHandler *m_frame;
    bool m_compress;
};

#endif // __EXPTHREAD_H__
