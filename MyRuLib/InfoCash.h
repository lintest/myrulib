#ifndef __INFOCASH_H__
#define __INFOCASH_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>
#include "FbConst.h"

#define INFO_CASH_SIZE 20

class InfoImage
{
public:
    InfoImage(const wxString &n, const wxSize &s): name(n), size(s) {};
public:
    wxString name;
    wxSize size;
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
    InfoImageArray images;
    bool loaded;
};

WX_DECLARE_OBJARRAY(InfoNode, InfoNodeArray);

class InfoCash
{
public:
    static void ShowInfo(wxEvtHandler *frame, const int id, const wxString &file_type);
    static wxString GetInfo(int id, bool vertical);
    static void Empty();
public:
    static void SetTitle(int id, wxString html);
    static void SetAnnotation(int id, wxString html);
    static void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
    static void SetLoaded(int id);
private:
    static InfoNodeArray sm_cash;
    static InfoNode * GetNode(int id);
	static wxCriticalSection sm_locker;
};

#endif // __INFOCASH_H__
