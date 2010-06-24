#include "FbViewData.h"
#include "FbColumns.h"
#include "FbDatabase.h"
#include "FbGenres.h"
#include "FbCollection.h"

#include <wx/buffer.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include "wx/base64.h"

//-----------------------------------------------------------------------------
//  FbViewData
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbViewDataArray);

//! Return specified string with the html special characters encoded.
//! Similar to PHP's htmlspecialchars() function.
wxString FbViewData::HTML( const wxString &value, const bool bSingleQuotes, const bool bDoubleQuotes )
{
  wxString szToReturn = value;
  szToReturn.Replace(wxT("&"),wxT("&amp;"));
  if( bSingleQuotes )
	szToReturn.Replace(wxT("'"),wxT("&#039;"));
  if( bDoubleQuotes )
	szToReturn.Replace(wxT("\""), wxT("&quot;"));
  szToReturn.Replace(wxT("<"),wxT("&lt;"));
  szToReturn.Replace(wxT(">"),wxT("&gt;"));
  return szToReturn;
}

FbViewData::~FbViewData()
{
	size_t count = m_images.Count();
	for (size_t i = 0; i < count; i++) {
		wxMemoryFSHandler::RemoveFile(GetImage(m_images[i]));
	}
}

void FbViewData::SetText(size_t index, const wxString &text)
{
	if (index < FILE) m_text[index] = text;
}

wxString FbViewData::GetText(size_t index) const
{
	if (index < FILE)
		return m_text[index];
	else
		return wxEmptyString;
}

wxString FbViewData::GetImage(const wxString &filename) const
{
	return wxString::Format(wxT("%d/%s"), m_id, filename.c_str());
}

void FbViewData::AddImage(wxString &filename, wxString &imagedata, wxString &imagetype)
{
	if (m_images.Index(filename) != wxNOT_FOUND) return;
	wxString imagename = GetImage(filename);
	wxMemoryBuffer buffer = wxBase64Decode(imagedata);
	wxMemoryInputStream stream(buffer.GetData(), buffer.GetDataLen());
	wxImage image(stream);
	if (image.GetWidth() <= MAX_IMAGE_WIDTH) {
		wxMemoryFSHandler::AddFile(imagename, buffer.GetData(), buffer.GetDataLen());
	} else {
		wxBitmap bitmap(image);
		int w = MAX_IMAGE_WIDTH;
		int h = bitmap.GetHeight() * MAX_IMAGE_WIDTH / bitmap.GetWidth();
		double scale = double(MAX_IMAGE_WIDTH) / bitmap.GetWidth();
		wxMemoryDC srcDC;
		srcDC.SelectObject(bitmap);

		wxBitmap result(w, h);
		wxMemoryDC memDC;
		memDC.SelectObject(result);
		memDC.SetUserScale(scale, scale);
		memDC.DrawBitmap(bitmap, 0, 0, false);
		memDC.SelectObject(wxNullBitmap);
		srcDC.SelectObject(wxNullBitmap);

		wxMemoryFSHandler::AddFile(imagename, result, wxBITMAP_TYPE_PNG);
	}
	m_images.Add(filename);
}

wxString FbViewData::GetComments(const FbViewContext &ctx, const FbCacheBook &book) const
{
	wxString md5sum = book.GetValue(BF_MD5S);
	if (md5sum.IsEmpty()) return wxEmptyString;

	wxString sql = wxT("SELECT id, posted, caption, comment FROM comments WHERE md5sum=? ORDER BY id");

	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, md5sum);
	wxSQLite3ResultSet res = stmt.ExecuteQuery();

	wxString html;
	html += wxT("<TABLE>");

	while (res.NextRow()) {
		int id = res.GetInt(0);
		wxString caption = HTML(res.GetString(2));
		wxString comment = HTML(res.GetString(3));
		int pos;
		while ( (pos = comment.Find(wxT('\n'))) != wxNOT_FOUND) {
			comment = comment.Left(pos) + wxT("<br>") + comment.Mid(pos + 1);
		}
		html += wxT("<TR><TD><B>");
		html += res.GetString(1) + wxT(" ") + caption;
		if (ctx.editable) {
			html += wxString::Format(wxT("&nbsp;<A href=%d target=M><IMG src=memory:modify></A>"), id);
			html += wxString::Format(wxT("&nbsp;<A href=%d target=D><IMG src=memory:delete></A>"), id);
		}
		html += wxT("</B></TD></TR>");
		html += wxString::Format(wxT("<TR><TD>%s</TD></TR>"), comment.c_str());
	}

	html += wxT("</TABLE>");

	return html;
}

wxString FbViewData::GetTitle(const FbCacheBook &book) const
{
	wxString html = wxString::Format(wxT("<font size=4><b>%s</b></font>"), HTML(book.GetValue(BF_AUTH)).c_str());

	wxString genres = book.GetValue(BF_GENR);
	if (!genres.IsEmpty()) html << wxString::Format(wxT("<br><font size=3>%s</font>"), HTML(genres).c_str());

	html << wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), HTML(book.GetValue(BF_NAME)).c_str());

	return html;
}

wxString FbViewData::GetHTML(const FbViewContext &ctx, const FbCacheBook &book) const
{

	wxString html = wxT("<table width=100%><tr>");
	wxString icon = FbCollection::GetIcon(ctx.filetype);
	if (icon.IsEmpty()) {
		html << wxString::Format(wxT("<td>%s</td>"), GetTitle(book).c_str());
	} else {
		icon = wxString::Format(wxT("<img src=\"memory:%s\">"), icon.c_str());
		html << wxT("<td><table cellspacing=0 cellpadding=0><tr>");
		html << wxString::Format(wxT("<td valign=top>%s&nbsp;&nbsp;&nbsp;</td>"), icon.c_str());
		html << wxString::Format(wxT("<td>%s</td>"), GetTitle(book).c_str());
		html << wxT("</tr></table></td>");
	}

	wxString isbn = GetText(ISBN);
	wxString file = GetText(FILE);
	wxString annt = GetText(ANNT);
	if (annt.IsEmpty()) annt = GetText(DSCR);

	if (ctx.vertical) {
		html << wxT("</tr>");
		html << wxString::Format(wxT("<tr><td>%s</td></tr>"), annt.c_str());
		for (size_t i=0; i<m_images.GetCount(); i++) {
			wxString image = GetImage(m_images[i]);
			html << wxT("<tr><td align=center>");
			html << wxT("<table border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><tr><td>");
			html << wxT("<table border=0 cellspacing=1 cellpadding=0 width=100%><tr><td bgcolor=#FFFFFF>");
			html << wxString::Format(wxT("<img src=\"memory:%s\">"), image.c_str());
			html << wxT("</td></tr></table>");
			html << wxT("</td></tr></table>");
			html << wxT("</td></tr>");
		}
		if (!isbn.IsEmpty()) {
			html << wxT("<tr><td align=center>");
			html << wxT("ISBN:&nbsp;") << isbn;
			html << wxT("</td></tr>");
		}
		html << wxString::Format(wxT("<tr><td>%s</td></tr>"), file.c_str());
		html << wxT("<tr><td valign=top>");
		html << GetComments(ctx, book);
		html << wxT("</td></tr>");
	} else {
		html << wxT("<td rowspan=4 align=right valign=top width=1>");
		html << wxT("<table width=100%><tr><td align=center>");
		for (size_t i=0; i<m_images.GetCount(); i++) {
			wxString image = GetImage(m_images[i]);
			html << wxT("<table border=0 cellspacing=0 cellpadding=0 bgcolor=#000000><tr><td>");
			html << wxT("<table border=0 cellspacing=1 cellpadding=0 width=100%><tr><td bgcolor=#FFFFFF>");
			html << wxString::Format(wxT("<img src=\"memory:%s\">"), image.c_str());
			html << wxT("</td></tr></table>");
			html << wxT("</td></tr></table>");
		}
		html << wxT("</td></tr>");
		if (!isbn.IsEmpty()) html << wxString::Format(wxT("<tr><td align=center>ISBN:&nbsp;%s</td></tr>"), isbn.c_str());
		html << wxT("</table></td></tr>");
		html << wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), annt.c_str());
		html << wxString::Format(wxT("<tr><td valign=top>%s</td></tr>"), file.c_str());
		html << wxT("<tr><td valign=top>");
		html << GetComments(ctx, book);
		html << wxT("</td></tr>");
	}
	html << wxT("</table></body>");

	return html;
}

