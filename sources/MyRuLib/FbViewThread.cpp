#include "FbViewThread.h"
#include "FbPreviewReader.h"
#include "FbExtractInfo.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbColumns.h"
#include "FbDatabase.h"
#include "ZipReader.h"
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

	ZipReader zip(id, false, true);
	if (zip.IsOk()) FbPreviewReader(*this, *info).Parse(zip.GetZip());
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

wxString FbViewThread::GetFiles(FbDatabase & database)
{
	FbExtractArray items(database, m_view.GetCode());

	wxString html;
	for (size_t i = 0; i < items.Count(); i++) {
		html << wxString::Format(wxT("<p>%s</p>"), items[i].ErrorName().c_str());
	}
	return html;
}

wxString FbViewThread::GetSeqns(FbDatabase & database)
{
	wxString html;
	wxString sql = wxT("select s.value, b.number FROM bookseq b INNER JOIN sequences s ON s.id=b.id_seq WHERE b.id_book="); 
	sql << m_view.GetCode();
	wxSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		wxString numb = res.GetString(1);
		html << wxT("<br>") << wxT("<font size=3>") << res.GetString(0);
		if (numb != wxT('0')) html << wxT(' ') << wxT('(') << numb << wxT(')');
		html << wxT("</font>");
	}
	return html;
}

