#include "FbViewData.h"
#include "FbViewThread.h"
#include "FbColumns.h"
#include "FbDatabase.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "MyRuLibApp.h"

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
	if (index < LAST_FIELD) m_text[index] = text;
}

wxString FbViewData::GetText(size_t index) const
{
	if (index < LAST_FIELD)
		return m_text[index];
	else
		return wxEmptyString;
}

wxString FbViewData::GetImage(const wxString &filename) const
{
	return wxString::Format(wxT("%d/%s"), m_id, filename.c_str());
}

void FbViewData::Push(const wxString &filename, const wxImage &image)
{
	int max = FbParams(FB_IMAGE_WIDTH);
	double scale = 1;
	int w = image.GetWidth();
	int h = image.GetHeight();
	if (w > max) {
		w = max;
		h = image.GetHeight() * max / image.GetWidth();
		scale = double(max) / image.GetWidth();
	}
	w += 2;
	h += 2;

	wxBitmap bitmap(image);
	wxMemoryDC srcDC;
	srcDC.SelectObject(bitmap);

	wxBitmap result(w, h);
	wxMemoryDC memDC;
	memDC.SelectObject(result);

	if (scale != 1) memDC.SetUserScale(scale, scale);
	memDC.Blit(1, 1, bitmap.GetWidth(), bitmap.GetHeight(), &srcDC, 0, 0, wxCOPY, true);

	wxPen pen( *wxBLACK, 1, wxSOLID );
	memDC.SetPen(pen);
	memDC.SetBrush(*wxTRANSPARENT_BRUSH);
	memDC.SetUserScale(1, 1);
	memDC.DrawRectangle(0, 0, w, h);

	memDC.SelectObject(wxNullBitmap);
	srcDC.SelectObject(wxNullBitmap);

	wxMemoryFSHandler::AddFile(filename, result, wxBITMAP_TYPE_PNG);
}

void FbViewData::AddImage(FbViewThread &thread, const wxString &filename, const wxString &imagedata)
{
	wxMemoryBuffer buffer = wxBase64Decode(imagedata, wxBase64DecodeMode_SkipWS);
	wxMemoryInputStream stream(buffer.GetData(), buffer.GetDataLen());
	AddImage(thread, filename, stream);
}

void FbViewData::AddImage(FbViewThread &thread, const wxString &filename, wxInputStream &stream)
{
	if (m_images.Index(filename) != wxNOT_FOUND) return;
	m_images.Add(filename);

	wxString imagename = GetImage(filename);
	wxImage image(stream);

#ifdef __WXMSW__
	Push(imagename, image);
	thread.SendHTML(*this);
#else
	FbImageEvent(wxID_ANY, image, m_id, imagename).Post(&wxGetApp());
#endif // __WXMSW__
}

wxString FbViewData::GetComments(const FbViewContext &ctx, const FbCacheBook &book) const
{
	wxString md5sum = book.GetValue(BF_MD5S);
	if (md5sum.IsEmpty()) return wxEmptyString;

	wxString sql = wxT("SELECT id, posted, caption, comment FROM comments WHERE md5sum=? ORDER BY id");

	FbLocalDatabase database;
	FbSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, md5sum);
	FbSQLite3ResultSet res = stmt.ExecuteQuery();

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

	html << wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), HTML(book.GetValue(BF_NAME)).c_str());
	html << GetText(SEQN);

	wxString genres = book.GetValue(BF_GENR);
	if (!genres.IsEmpty()) html << wxString::Format(wxT("<br><font size=3>%s</font>"), HTML(genres).c_str());

	return html;
}

wxString FbViewData::GetHTML(const FbViewContext &ctx, const FbCacheBook &book) const
{

	wxString html = wxT("<table width=100%><tr>");
	wxString icon = GetText(ICON);
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
			html << wxString::Format(wxT("<img src=\"memory:%s\">"), image.c_str());
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
		html << wxT("<table width=100%>");
		for (size_t i=0; i<m_images.GetCount(); i++) {
			wxString image = GetImage(m_images[i]);
			html << wxT("<tr><td align=center>");
			html << wxString::Format(wxT("<img src=\"memory:%s\">"), image.c_str());
			html << wxT("</td></tr>");
		}
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

