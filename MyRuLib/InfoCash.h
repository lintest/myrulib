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
    InfoNode(): id(0), loaded(false) {};
    virtual ~InfoNode();
    void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
public:
    int id;
    wxString title;
    wxString annotation;
    wxString filelist;
    InfoImageArray images;
    bool loaded;
};

WX_DECLARE_OBJARRAY(InfoNode, InfoNodeArray);

class InfoCash
{
public:
    static void UpdateInfo(wxEvtHandler *frame, const int id, const wxString &file_type);
    static void LoadInfo(wxHtmlWindow * bookinfo, const int id, bool vertical);
    static void Empty();
public:
    static void SetTitle(int id, wxString html);
    static void SetFilelist(int id, wxString html);
    static void SetAnnotation(int id, wxString html);
    static void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
    static void SetLoaded(int id);
private:
    static InfoNodeArray sm_cash;
    static InfoNode * GetNode(int id);
	static wxCriticalSection sm_locker;
};

#endif // __INFOCASH_H__
