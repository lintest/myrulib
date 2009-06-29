#include "InfoCash.h"

#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include "wx/base64.h"

#include "InfoThread.h"

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

InfoNode * InfoCash::GetNode(int id)
{
    InfoNodeArray * cash = GetCash();
    for (size_t i=0; i<cash->GetCount(); i++) {
        if (cash->Item(i).id == id) return &cash->Item(i);
    }
    while ( cash->GetCount()>INFO_CASH_SIZE ) {
        cash->RemoveAt(INFO_CASH_SIZE);
    }
    InfoNode * node = new InfoNode;
    node->id = id;
    cash->Insert(node, 0);
    return node;
}

InfoNodeArray * InfoCash::GetCash()
{
    static InfoNodeArray array;
    return &array;
}

static wxCriticalSection infoLocker;

void InfoCash::AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype)
{
    wxCriticalSectionLocker enter(infoLocker);
    InfoNode * node = GetNode(id);
    node->AddImage(id, filename, imagedata, imagetype);
}

void InfoCash::SetTitle(int id, wxString html)
{
    wxCriticalSectionLocker enter(infoLocker);
    GetNode(id)->title = html;
};

void InfoCash::SetAnnotation(int id, wxString html)
{
    wxCriticalSectionLocker enter(infoLocker);
    GetNode(id)->annotation = html;
};

wxString InfoCash::GetInfo(int id, bool vertical)
{
    if (!id) return wxEmptyString;

    wxCriticalSectionLocker enter(infoLocker);
    InfoNode * node = GetNode(id);

    wxString html = wxT("<html><body><table>");

    html += wxString::Format(wxT("<tr><td>%s</td></tr>"), node->title.c_str());
    html += wxString::Format(wxT("<tr><td>%s</td></tr>"), node->annotation.c_str());
    for (size_t i=0; i<node->images.GetCount(); i++) {
        html += wxString::Format(wxT("<tr><td align=center><img src=\"memory:%s\"></td></tr>"), node->images[i].name.c_str());
    }
    html += wxT("</table></body></html>");

    return html;
}

void InfoCash::ShowInfo(wxEvtHandler *frame, const int id)
{
    InfoNode * node = GetNode(id);
    if (node->loaded) {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
        event.SetInt(id);
        wxPostEvent(frame, event);
    } else {
        TitleThread::Execute(id);
        InfoThread::Execute(id);
        node->loaded = true;
    }
}
