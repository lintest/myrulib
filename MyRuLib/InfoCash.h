#ifndef __INFOCASH_H__
#define __INFOCASH_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>

#define INFO_CASH_SIZE 20

#define ID_BOOKINFO_UPDATE 40000

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
    static void ShowInfo(wxEvtHandler *frame, const int id);
    static wxString GetInfo(int id, bool vertical = false);
public:
    static void SetTitle(int id, wxString html);
    static void SetAnnotation(int id, wxString html);
    static void AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype);
private:
    static InfoNodeArray * GetCash();
    static InfoNode * GetNode(int id);
};

#endif // __INFOCASH_H__
