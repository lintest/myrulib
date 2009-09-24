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

wxCriticalSection InfoCash::sm_locker;

void InfoCash::SetLoaded(int id)
{
    wxCriticalSectionLocker enter(sm_locker);
    for (size_t i=0; i<sm_cash.GetCount(); i++) {
        if (sm_cash.Item(i).id == id) {
            InfoNode * node = sm_cash.Detach(i);
            node->loaded = true;
            return ;
        }
    }
}

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

void InfoCash::SetFilelist(int id, wxString html)
{
    wxCriticalSectionLocker enter(sm_locker);
    GetNode(id)->filelist = html;
};

void InfoCash::Empty()
{
    wxCriticalSectionLocker enter(sm_locker);
    sm_cash.Empty();
}

class ShowThread: public wxThread
{
	public:
		ShowThread(wxEvtHandler *frame, int id): m_frame(frame), m_id(id) {};
		virtual void *Entry() {
			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
			event.SetInt(m_id);
			wxPostEvent(m_frame, event);
			return NULL;
		};
		static void Execute(wxEvtHandler *frame, const int id)
		{
			if (!id) return;
			TitleThread *thread = new TitleThread(frame, id);
			if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
		};
	private:
		wxEvtHandler * m_frame;
		int m_id;

};

void InfoCash::UpdateInfo(wxEvtHandler *frame, const int id, const wxString &file_type)
{
    if (!id) return;

    wxCriticalSectionLocker enter(sm_locker);
    InfoNode * node = GetNode(id);

    if (node->loaded) {
    	ShowThread::Execute(frame, id);
    } else {
        TitleThread::Execute(frame, id);
        if (file_type == wxT("fb2")) InfoThread::Execute(frame, id);
        node->loaded = true;
    }
}

void InfoCash::LoadInfo(wxHtmlWindow * bookinfo, const int id, bool vertical)
{
    if (!id) return;
    wxCriticalSectionLocker enter(sm_locker);

    InfoNode * node = GetNode(id);

    wxString html = wxT("<html><body><table width=100%>");

    if (vertical) {
        html += wxString::Format(wxT("<tr><td width=100%>%s</td></tr>"), node->title.c_str());
        html += wxString::Format(wxT("<tr><td>%s</td></tr>"), node->annotation.c_str());
        for (size_t i=0; i<node->images.GetCount(); i++) {
            InfoImage & info = node->images[i];
            html += wxString::Format(wxT("<tr><td align=center><img src=\"memory:%s\" width=%d height=%d></td></tr>"), info.GetName().c_str(), info.GetWidth(), info.GetHeight());
        }
        html += wxString::Format(wxT("<tr><td>%s</td></tr>"), node->filelist.c_str());
    } else {
        html += wxT("<tr width=100%>");
        html += wxString::Format(wxT("<td>%s</td>"), node->title.c_str());
        html += wxT("<td rowspan=3 align=right valign=top>");
        for (size_t i=0; i<node->images.GetCount(); i++) {
            InfoImage & info = node->images[i];
            html += wxString::Format(wxT("<img src=\"memory:%s\" width=%d height=%d><br>"), info.GetName().c_str(), info.GetWidth(), info.GetHeight());
        }
        html += wxT("</td></tr>");
        html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), node->annotation.c_str());
        html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), node->filelist.c_str());
    }
    html += wxT("</table></body></html>");

    bookinfo->SetPage(html);
}
