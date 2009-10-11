#ifndef __INFOCASH_H__
#define __INFOCASH_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>
#include <wx/html/htmlwin.h>
#include "FbConst.h"

#define INFO_CASH_SIZE 20

class InfoImage
{
public:
    InfoImage(const wxString &name, const wxImage &image);
public:
    const wxString & GetName() { return m_name; };
    const int GetWidth();
    const int GetHeight();
private:
    wxString m_name;
    int m_width;
    int m_height;
};

WX_DECLARE_OBJARRAY(InfoImage, InfoImageArray);

class InfoNode
{
public:
    InfoNode(): id(0) {};
    virtual ~InfoNode();
    void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
public:
    int id;
    wxString title;
    wxString annotation;
    wxString filelist;
    InfoImageArray images;
};

WX_DECLARE_OBJARRAY(InfoNode, InfoNodeArray);

class InfoCash
{
public:
    static void UpdateInfo(wxEvtHandler *frame, const int id, const wxString &file_type);
    static wxString GetInfo(const int id, bool vertical);
    static void Empty();
public:
    static void SetTitle(int id, wxString html);
    static void SetFilelist(int id, wxString html);
    static void SetAnnotation(int id, wxString html);
    static void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
private:
    static InfoNodeArray sm_cash;
    static InfoNode * GetNode(int id);
    static InfoNode * FindNode(int id);
	static wxCriticalSection sm_locker;
};

class ShowThread: public wxThread
{
	public:
		ShowThread(wxEvtHandler *frame, int id): m_frame(frame), m_id(id) {};
		virtual void * Entry();
		static void Execute(wxEvtHandler *frame, const int id);
	private:
		wxEvtHandler * m_frame;
		int m_id;
};

#endif // __INFOCASH_H__
