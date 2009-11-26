#include "InfoCash.h"

#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include <wx/mimetype.h>
#include "wx/base64.h"

#include "InfoThread.h"
#include "TitleThread.h"

#include "res/ico_pdf.xpm"
#include "res/ico_djvu.xpm"

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
	for (size_t i=0; i<m_images.GetCount(); i++) {
		wxMemoryFSHandler::RemoveFile(m_images[i].GetName());
	}
}

void InfoNode::AddImage(int id, wxString &filename, wxString &imagedata, wxString &imagetype)
{
	wxString imagename = wxString::Format(wxT("%d/%s"), id, filename.c_str());
	for (size_t i=0; i<m_images.GetCount(); i++) {
		if (m_images[i].GetName() == imagename) return;
	}
	wxMemoryBuffer buffer = wxBase64Decode(imagedata);
	wxMemoryFSHandler::AddFileWithMimeType(imagename, buffer.GetData(), buffer.GetDataLen(), imagetype);
	wxMemoryInputStream stream(buffer.GetData(), buffer.GetDataLen());
	wxImage image(stream);
	m_images.Add(new InfoImage(imagename, image));
}

//-----------------------------------------------------------------------------
//  InfoCash
//-----------------------------------------------------------------------------

InfoNodeArray InfoCash::sm_cash;

wxArrayString InfoCash::sm_icons;
wxArrayString InfoCash::sm_noico;

InfoNode * InfoCash::GetNode(int id)
{
	for (size_t i=0; i<sm_cash.GetCount(); i++) {
		if (sm_cash.Item(i).m_id == id) {
			InfoNode * node = sm_cash.Detach(i);
			sm_cash.Insert(node, 0);
			return node;
		}
	}

	while ( sm_cash.GetCount()>INFO_CASH_SIZE ) {
		sm_cash.RemoveAt(INFO_CASH_SIZE);
	}

	InfoNode * node = new InfoNode;
	node->m_id = id;
	sm_cash.Insert(node, 0);
	return node;
}

InfoNode * InfoCash::FindNode(int id)
{
	for (size_t i=0; i<sm_cash.GetCount(); i++) {
		if (sm_cash.Item(i).m_id == id) {
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

void InfoCash::EmptyInfo(int id)
{
	wxCriticalSectionLocker enter(sm_locker);

	for (size_t i=0; i<sm_cash.GetCount(); i++) {
		if (sm_cash.Item(i).m_id == id) {
			sm_cash.RemoveAt(i);
			return;
		}
	}
}

void InfoCash::SetTitle(int id, wxString html)
{
	wxCriticalSectionLocker enter(sm_locker);
	GetNode(id)->m_title = html;
};

void InfoCash::SetISBN(int id, wxString html)
{
	wxCriticalSectionLocker enter(sm_locker);
	GetNode(id)->m_isbn = html;
};

void InfoCash::SetAnnotation(int id, wxString html)
{
	wxCriticalSectionLocker enter(sm_locker);
	InfoNode * node = FindNode(id);
	if (node) node->m_annotation = html;
};

void InfoCash::SetFilelist(int id, wxString html)
{
	wxCriticalSectionLocker enter(sm_locker);
	InfoNode * node = FindNode(id);
	if (node) node->m_filelist = html;
};

void InfoCash::Empty()
{
	wxCriticalSectionLocker enter(sm_locker);
	sm_cash.Empty();
}

void InfoCash::UpdateInfo(wxEvtHandler *frame, int id, bool bVertical, bool bEditable)
{
	if (id) (new ShowThread(frame, id, bVertical, bEditable))->Execute();
}

wxString InfoCash::GetInfo(int id, const wxString &md5sum, bool bVertical, bool bEditable, const wxString &filetype)
{
	wxCriticalSectionLocker enter(sm_locker);

	InfoNode * node = FindNode(id);
	if (node)
		return node->GetHTML(md5sum, bVertical, bEditable, filetype);
	else
		return wxEmptyString;
}

void InfoCash::AddIcon(wxString extension, wxBitmap bitmap)
{
	wxString filename = wxT("icon.") + extension;
	wxMemoryFSHandler::AddFile(filename, bitmap, wxBITMAP_TYPE_PNG);
	sm_icons.Add(extension);
}

void InfoCash::LoadIcon(const wxString &extension)
{
 	if (extension == wxT("fb2")) return;
	wxString filename = wxT("icon.") + extension;

	wxCriticalSectionLocker enter(sm_locker);

	if (!sm_icons.Count()) {
		AddIcon((wxString)wxT("djvu"), wxBitmap(ico_djvu_xpm));
		AddIcon((wxString)wxT("pdf"), wxBitmap(ico_pdf_xpm));
	}

	if (sm_icons.Index(extension) != wxNOT_FOUND) return;
	if (sm_noico.Index(extension) != wxNOT_FOUND) return;

	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(extension);
	if ( ft ) {
		wxIconLocation location;
		if ( ft->GetIcon(&location) ) {
			wxLogNull log;
			wxIcon icon(location);
			wxBitmap bitmap;
			bitmap.CopyFromIcon(icon);
			wxMemoryFSHandler::AddFile(filename, bitmap, wxBITMAP_TYPE_PNG);
			sm_icons.Add(extension);
			return;
		}
	}
	sm_noico.Add(extension);
}

wxString InfoCash::GetIcon(const wxString &extension)
{
	wxString filename = wxT("icon.") + extension;
	if (sm_icons.Index(extension) != wxNOT_FOUND)
		return filename;
	else
		return wxEmptyString;
}

wxString InfoNode::GetComments(const wxString md5sum, bool bEditable)
{
	wxString sql = wxT("SELECT id, posted, caption, comment FROM comments WHERE md5sum=? ORDER BY id");

	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, md5sum);
	wxSQLite3ResultSet res = stmt.ExecuteQuery();

	wxString html;
	html += wxT("<TABLE>");

	while (res.NextRow()) {
		int id = res.GetInt(0);
		wxString caption = FbBookThread::HTMLSpecialChars(res.GetString(2));
		wxString comment = FbBookThread::HTMLSpecialChars(res.GetString(3));
		int pos;
		while ( (pos = comment.Find(wxT('\n'))) != wxNOT_FOUND) {
			comment = comment.Left(pos) + wxT("<br>") + comment.Mid(pos + 1);
		}
		html += wxT("<TR><TD><B>");
		html += res.GetString(1) + wxT(" ") + caption;
		if (bEditable) {
			html += wxString::Format(wxT("&nbsp;<A href=%d target=M><IMG src=memory:modify></A>"), id);
			html += wxString::Format(wxT("&nbsp;<A href=%d target=D><IMG src=memory:delete></A>"), id);
		}
		html += wxT("</B></TD></TR>");
		html += wxString::Format(wxT("<TR><TD>%s</TD></TR>"), comment.c_str());
	}

	html += wxT("</TABLE>");

	return html;
}

wxString InfoNode::GetHTML(const wxString &md5sum, bool bVertical, bool bEditable, const wxString &filetype)
{
	wxString html = wxT("<html><body><table width=100%>");

	html += wxT("<tr>");
	wxString icon = InfoCash::GetIcon(filetype);
	if (icon.IsEmpty()) {
		html += wxString::Format(wxT("<td>%s</td>"), m_title.c_str());
	} else {
		icon = wxString::Format(wxT("<img src=\"memory:%s\">"), icon.c_str());
		html += wxT("<td><table cellspacing=0 cellpadding=0><tr>");
		html += wxString::Format(wxT("<td valign=top>%s&nbsp;&nbsp;&nbsp;</td>"), icon.c_str());
		html += wxString::Format(wxT("<td>%s</td>"), m_title.c_str());
		html += wxT("</tr></table></td>");
	}

	if (bVertical) {
		html += wxT("</tr>");
		html += wxString::Format(wxT("<tr><td>%s</td></tr>"), m_annotation.c_str());
		for (size_t i=0; i<m_images.GetCount(); i++) {
			InfoImage & info = m_images[i];
			html += wxT("<tr><td align=center>");
			html += wxT("<table border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><tr><td>");
			html += wxT("<table border=0 cellspacing=1 cellpadding=0 width=100%><tr><td bgcolor=#FFFFFF>");
			html += wxString::Format(wxT("<img src=\"memory:%s\" width=%d height=%d>"), info.GetName().c_str(), info.GetWidth(), info.GetHeight());
			html += wxT("</td></tr></table>");
			html += wxT("</td></tr></table>");
			html += wxT("</td></tr>");
		}
		if (!m_isbn.IsEmpty()) {
			html += wxT("<tr><td align=center>");
			html += wxT("ISBN:&nbsp;") + m_isbn;
			html += wxT("</td></tr>");
		}
		html += wxString::Format(wxT("<tr><td>%s</td></tr>"), m_filelist.c_str());
		html += wxT("<tr><td valign=top>");
		html += GetComments(md5sum, bEditable);
		html += wxT("</td></tr>");
	} else {
		html += wxT("<td rowspan=4 align=right valign=top width=1>");
		html += wxT("<table width=100%><tr><td align=center>");
		for (size_t i=0; i<m_images.GetCount(); i++) {
			InfoImage & info = m_images[i];
			html += wxT("<table border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><tr><td>");
			html += wxT("<table border=0 cellspacing=1 cellpadding=0 width=100%><tr><td bgcolor=#FFFFFF>");
			html += wxString::Format(wxT("<img src=\"memory:%s\" width=%d height=%d>"), info.GetName().c_str(), info.GetWidth(), info.GetHeight());
			html += wxT("</td></tr></table>");
			html += wxT("</td></tr></table>");
		}
		html += wxT("</td></tr>");
		if (!m_isbn.IsEmpty()) html += wxString::Format(wxT("<tr><td align=center>ISBN:&nbsp;%s</td></tr>"), m_isbn.c_str());
		html += wxT("</table></td></tr>");
		html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), m_annotation.c_str());
		html += wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), m_filelist.c_str());
		html += wxT("<tr><td valign=top>");
		html += GetComments(md5sum, bEditable);
		html += wxT("</td></tr>");
	}
	html += wxT("</table></body></html>");

	return html;
}

void * ShowThread::Entry()
{
	if (!m_id) return NULL;

	wxString sql = wxT("SELECT file_type, md5sum FROM books WHERE id=?");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_id);
	wxSQLite3ResultSet res = stmt.ExecuteQuery();
	if (res.NextRow()) {
		m_filetype = res.GetString(0);
		m_md5sum = res.GetString(1);
	}

	wxString html = InfoCash::GetInfo(m_id, m_md5sum, m_vertical, m_editable, m_filetype);

	if (html.IsEmpty()) {
		(new TitleThread(this))->Execute();
		(new InfoThread(this))->Execute();
	} else {
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
		event.SetInt(m_id);
		event.SetString(html);
		wxPostEvent(m_frame, event);
	}
	return NULL;
}
