#include "InfoCash.h"

#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include "wx/base64.h"

#include "InfoThread.h"
#include "TitleThread.h"

WX_DEFINE_OBJARRAY(InfoImageArray);

WX_DEFINE_OBJARRAY(InfoNodeArray);

//-----------------------------------------------------------------------------
//  InfoNode
//-----------------------------------------------------------------------------

InfoImage::InfoImage(const wxString &name, const wxImage &image)
    : m_name(name), m_width(image.GetWidth()), m_height(image.GetHeight())

{
}

const int InfoImage::GetWidth()
{
    return m_width > ciMaxImageWidth ? ciMaxImageWidth : m_width;
}

const int InfoImage::GetHeight()
{
    return m_width > ciMaxImageWidth ? m_height * ciMaxImageWidth / m_width : m_height;
}

InfoNode::~InfoNode()
{
    for (size_t i=0; i<images.GetCount(); i++) {
        wxMemoryFSHandler::RemoveFile(images[i].GetName());
    }
}

void InfoNode::AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype)
{
    wxString imagename = wxString::Format(wxT("%d/%s"), id, filename.c_str());
    for (size_t i=0; i<images.GetCount(); i++) {
        if (images[i].GetName() == imagename) return;
    }
    wxMemoryBuffer buffer = wxBase64Decode(imagedata);
	wxMemoryFSHandler::AddFileWithMimeType(imagename, buffer.GetData(), buffer.GetDataLen(), imagetype);
	wxMemoryInputStream stream(buffer.GetData(), buffer.GetDataLen());
	wxImage image(stream);
    images.Add(new InfoImage(imagename, image));
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

InfoNode * InfoCash::FindNode(int id)
{
    for (size_t i=0; i<sm_cash.GetCount(); i++) {
        if (sm_cash.Item(i).id == id) {
            InfoNode * node = sm_cash.Detach(i);
            sm_cash.Insert(node, 0);
            return node;
        }
    }
    return NULL;
}

wxCriticalSection InfoCash::sm_locker;

void InfoCash::AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype)
{
    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = FindNode(id);
    if (node) node->AddImage(id, filename, imagedata, imagetype);
}

void InfoCash::SetTitle(int id, wxString html)
{
    wxCriticalSectionLocker enter(sm_locker);
    GetNode(id)->title = html;
};

void InfoCash::SetAnnotation(int id, wxString html)
{
    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = FindNode(id);
    if (node) node->annotation = html;
};

void InfoCash::SetFilelist(int id, wxString html)
{
    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = FindNode(id);
    if (node) node->filelist = html;
};

void InfoCash::Empty()
{
    wxCriticalSectionLocker enter(sm_locker);
    sm_cash.Empty();
}

void InfoCash::UpdateInfo(wxEvtHandler *frame, const int id, const wxString &file_type, const bool vertical)
{
    if (!id) return;

    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = FindNode(id);
    if (node) {
		wxThread *thread = new ShowThread(frame, id, vertical);
		if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
    } else {
        TitleThread::Execute(frame, id, vertical);
        if (file_type == wxT("fb2")) InfoThread::Execute(frame, id, vertical);
    }
}

wxString InfoCash::GetInfo(const int id, bool vertical)
{
    if (!id) return wxEmptyString;
    wxCriticalSectionLocker enter(sm_locker);

    InfoNode * node = FindNode(id);
    if (node)
		return node->GetHTML(vertical);
	else
		return wxEmptyString;
}

wxString InfoNode::GetHTML(bool vertical)
{
    wxString html = wxT("<html><body><table width=100%>");

    if (vertical) {
        html += wxString::Format(wxT("<tr><td width=100%>%s</td></tr>"), title.c_str());
        html += wxString::Format(wxT("<tr><td>%s</td></tr>"), annotation.c_str());
        for (size_t i=0; i<images.GetCount(); i++) {
            InfoImage & info = images[i];
            html += wxT("<tr><td align=center>");
			html += wxT("<table border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><tr><td>");
			html += wxT("<table border=0 cellspacing=1 cellpadding=0 width=100%><tr><td bgcolor=#FFFFFF>");
            html += wxString::Format(wxT("<img src=\"memory:%s\" width=%d height=%d>"), info.GetName().c_str(), info.GetWidth(), info.GetHeight());
            html += wxT("</td></tr></table>");
            html += wxT("</td></tr></table>");
            html += wxT("</td></tr>");
        }
        html += wxString::Format(wxT("<tr><td>%s</td></tr>"), filelist.c_str());
    } else {
        html += wxT("<tr width=100%>");
        html += wxString::Format(wxT("<td>%s</td>"), title.c_str());
        html += wxT("<td rowspan=3 align=right valign=top>");
        for (size_t i=0; i<images.GetCount(); i++) {
            InfoImage & info = images[i];
			html += wxT("<table border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><tr><td>");
			html += wxT("<table border=0 cellspacing=1 cellpadding=0 width=100%><tr><td bgcolor=#FFFFFF>");
            html += wxString::Format(wxT("<img src=\"memory:%s\" width=%d height=%d>"), info.GetName().c_str(), info.GetWidth(), info.GetHeight());
            html += wxT("</td></tr></table>");
            html += wxT("</td></tr></table>");
        }
        html += wxT("</td></tr>");
        html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), annotation.c_str());
        html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), filelist.c_str());
    }
//    html += wxT("</table>");
//    html += wxT("</table></body></html>");

	return html;
}

void * ShowThread::Entry()
{
	wxString html = InfoCash::GetInfo(m_id, m_vertical);
	wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
	event.SetInt(m_id);
	event.SetString(html);
	wxPostEvent(m_frame, event);
	return NULL;
}

