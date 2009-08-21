#include "InfoCash.h"

#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include "wx/base64.h"

#include "InfoThread.h"
#include "TitleThread.h"

WX_DEFINE_OBJARRAY(InfoImageArray);

WX_DEFINE_OBJARRAY(InfoNodeArray);

//-----------------------------------------------------------------------------
//  InfoNode
//-----------------------------------------------------------------------------

InfoNode::~InfoNode()
{
    for (size_t i=0; i<images.GetCount(); i++) {
        wxMemoryFSHandler::RemoveFile(images[i].name);
    }
}

void InfoNode::AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype)
{
    wxString imagename = wxString::Format(wxT("%d/%s"), id, filename.c_str());
    for (size_t i=0; i<images.GetCount(); i++) {
        if (images[i].name == imagename) return;
    }
    wxMemoryBuffer buffer = wxBase64Decode(imagedata);
    wxMemoryFSHandler::AddFileWithMimeType(imagename, buffer.GetData(), buffer.GetDataLen(), imagetype);
    images.Add(new InfoImage(imagename, wxDefaultSize));
}

//-----------------------------------------------------------------------------
//  InfoCash
//-----------------------------------------------------------------------------

InfoNodeArray InfoCash::sm_cash;

InfoNode * InfoCash::GetNode(int id)
{
    for (size_t i=0; i<sm_cash.GetCount(); i++) {
        if (sm_cash.Item(i).id == id) {
            InfoNode * node = sm_cash.Detach(i);
            sm_cash.Insert(node, 0);
            return node;
        }
    }
    while ( sm_cash.GetCount()>INFO_CASH_SIZE ) {
        sm_cash.RemoveAt(INFO_CASH_SIZE);
    }
    InfoNode * node = new InfoNode;
    node->id = id;
    sm_cash.Insert(node, 0);
    return node;
}

void InfoCash::SetLoaded(int id)
{
    for (size_t i=0; i<sm_cash.GetCount(); i++) {
        if (sm_cash.Item(i).id == id) {
            InfoNode * node = sm_cash.Detach(i);
            node->loaded = true;
            return ;
        }
    }
}

wxCriticalSection InfoCash::sm_locker;

void InfoCash::AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype)
{
    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = GetNode(id);
    node->AddImage(id, filename, imagedata, imagetype);
}

void InfoCash::SetTitle(int id, wxString html)
{
    wxCriticalSectionLocker enter(sm_locker);
    GetNode(id)->title = html;
};

void InfoCash::SetAnnotation(int id, wxString html)
{
    wxCriticalSectionLocker enter(sm_locker);
    GetNode(id)->annotation = html;
};

wxString InfoCash::GetInfo(int id, bool vertical)
{
    if (!id) return wxEmptyString;

    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = GetNode(id);

    wxString html = wxT("<html><body><table width=100%>");

    if (vertical) {
        html += wxString::Format(wxT("<tr><td width=100%>%s</td></tr>"), node->title.c_str());
        html += wxString::Format(wxT("<tr><td>%s</td></tr>"), node->annotation.c_str());
        for (size_t i=0; i<node->images.GetCount(); i++) {
            html += wxString::Format(wxT("<tr><td align=center><img src=\"memory:%s\"></td></tr>"), node->images[i].name.c_str());
        }
    } else {
        html += wxString::Format(wxT("<tr width=100%><td>%s</td>"), node->title.c_str());
        html += wxT("<td rowspan=2 align=right valign=top>");
        for (size_t i=0; i<node->images.GetCount(); i++) {
            html += wxString::Format(wxT("<img src=\"memory:%s\"><br>"), node->images[i].name.c_str());
        }
        html += wxT("</td></tr>");
        html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), node->annotation.c_str());
    }
    html += wxT("</table></body></html>");

    return html;
}

void InfoCash::ShowInfo(wxEvtHandler *frame, const int id)
{
    if (!id) return;

    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = GetNode(id);

    if (node->loaded) {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
        event.SetInt(id);
        wxPostEvent(frame, event);
    } else {
        TitleThread::Execute(frame, id);
        InfoThread::Execute(frame, id);
    }
}
