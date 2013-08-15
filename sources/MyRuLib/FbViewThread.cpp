#include "FbViewThread.h"
#include "FbInternetBook.h"
#include "FbFileReader.h"
#include "FbPreviewReader.h"
#include "FbExtractInfo.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbColumns.h"
#include "FbDatabase.h"
#include "FbString.h"
#include "FbParams.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbViewThread
//-----------------------------------------------------------------------------

void * FbViewThread::Entry()
{
	try {
		switch (m_view.GetType()) {
			case FbViewItem::Book: OpenBook(); break;
			case FbViewItem::Auth: OpenAuth(); break;
			default: OpenNone();
		}
	} catch (...) {}
	return NULL;
}

void FbViewThread::OpenAuth()
{
	wxString info = FbCommonDatabase().Str(m_view.GetCode(), wxT("SELECT description FROM authors WHERE id=? AND description IS NOT NULL"));
	if (!info.IsEmpty()) SendHTML(ID_AUTH_PREVIEW, info);
}

void FbViewThread::OpenBook()
{
	int id = m_view.GetCode();
	m_book = FbCollection::GetBookData(id);
	if (!m_book) {
		SendHTML(ID_BOOK_PREVIEW);
		return;
	}

	if (IsClosed()) return;

	wxString html = FbCollection::GetBookHTML(m_ctx, m_book, id);
	if (!html.IsEmpty()) {
		SendHTML(ID_BOOK_PREVIEW, html);
		return;
	}

	if (IsClosed()) return;

	FbViewData * info = new FbViewData(id);
	{
		wxString filetype = m_book.GetValue(BF_TYPE);
		FbCommonDatabase database;
		database.JoinThread(this);
		info->SetText(FbViewData::DSCR, GetDescr(database));
		info->SetText(FbViewData::FILE, GetFiles(database));
		info->SetText(FbViewData::SEQN, GetSeqns(database));
		info->SetText(FbViewData::ICON, FbCollection::GetIcon(filetype));
		SendHTML(*info);
	}

	if (IsClosed()) { delete info; return; }

	FbFileReader file(id, true);
	if (file.IsOk()) {
		wxInputStream & in = file.GetStream();
		if (file.GetFileType() == wxT("epub")) {
			FbPreviewReaderEPUB(*this, *info).Preview(in);
		} else {
			FbPreviewReader(*this, *info).Parse(in);
		}
	}
	FbCollection::AddInfo(info);
}

void FbViewThread::OpenNone()
{
	SendHTML(ID_BOOK_PREVIEW);
}

void FbViewThread::SendHTML(const FbViewData &info)
{
	wxString html = info.GetHTML(m_ctx, m_book);
	SendHTML(ID_BOOK_PREVIEW, html);
}

void FbViewThread::SendHTML(wxWindowID winid, const wxString &html)
{
	FbCommandEvent(fbEVT_BOOK_ACTION, winid, m_view.GetCode(), html).Post(m_frame);
}

wxString FbViewThread::GetDescr(FbDatabase & database)
{
	return database.Str(m_view.GetCode(), wxT("SELECT description FROM books WHERE id=? AND description IS NOT NULL"));
}

static wxString Shorten(const wxString &filename, int length)
{
	if (filename.Len() <= (size_t)length) return filename;

	wxString ext;
	int pos = filename.Find(wxT('.'), true);
	if (pos != wxNOT_FOUND) ext = filename.Mid(pos);

	FbString result = filename;
	result.Truncate(pos);
	result = result.AfterLast(wxT('/'));
	return result.Shorten(length) << ext;
}

wxString FbViewThread::GetFiles(FbDatabase & database)
{
	int maxLength = FbParams(FB_FILE_LENGTH);
	int id = m_view.GetCode();
	wxString html;
	wxString sql = wxT("SELECT DISTINCT id_archive,file_name,1,id_archive*id_archive,file_type,md5sum FROM books WHERE id=?1 UNION ");
	sql << wxT("SELECT DISTINCT id_archive,file_name,2,id_archive*id_archive,NULL,NULL FROM files WHERE id_book=?1 ORDER BY 3,4");
	FbSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	FbSQLite3ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		html << wxT("<p>");
		if (id > 0 && result.GetInt(2) == 1) {
			wxString lib = FbParams(FB_CONFIG_TYPE);
			wxString md5 = result.GetAsString(wxT("md5sum"));
			wxString ext = result.GetAsString(wxT("file_type"));
			wxString str = wxString::Format(wxT("$(%s)/%d.%s"), lib.c_str(), id, ext.c_str());
			wxString url = FbInternetBook::GetURL(id, md5);
			html << wxString::Format(wxT("<a href=\"%s\">%s</a>"), url.c_str(), str.c_str());
		} else {
			html << wxT("<p>");
			if (int arch = result.GetInt(0)) {
				wxString url;
				wxString trg = result.GetString(1);
				wxString str = Shorten(result.GetString(1), maxLength);
				wxString sql = wxT("SELECT file_name FROM archives WHERE id="); sql << arch;
				FbSQLite3ResultSet result = database.ExecuteQuery(sql);
				if (result.NextRow()) {
					url = wxT("book:") + result.GetString(0);
					wxString str = Shorten(result.GetString(0), maxLength);
					html << wxString::Format(wxT("<a href=\"%s\">%s</a>"), url.c_str(), str.c_str());
					html << wxT(": ");
				}
				html << wxString::Format(wxT("<a href=\"%s\" target=\"%s\">%s</a>"), url.c_str(), trg.c_str(), str.c_str());
			} else {
				wxString url = wxT("book:") + result.GetString(1);
				wxString str = Shorten(result.GetString(1), maxLength);
				html << wxString::Format(wxT("<a href=\"%s\">%s</a>"), url.c_str(), str.c_str());
			}
		}
		html << wxT("</p>");
	}
	return html;
}

wxString FbViewThread::GetSeqns(FbDatabase & database)
{
	wxString html;
	wxString sql = wxT("select s.value, b.number FROM bookseq b INNER JOIN sequences s ON s.id=b.id_seq WHERE b.id_book=");
	sql << m_view.GetCode();
	FbSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		wxString numb = res.GetString(1);
		html << wxT("<br>") << wxT("<font size=3>") << res.GetString(0);
		if (numb != wxT('0')) html << wxT(' ') << wxT('(') << numb << wxT(')');
		html << wxT("</font>");
	}
	return html;
}

