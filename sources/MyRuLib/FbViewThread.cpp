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
	wxString sql = wxT("SELECT description FROM authors WHERE id=? AND description IS NOT NULL");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_view.GetCode());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) SendHTML(ID_AUTH_PREVIEW, result.GetString(0));
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
		info->SetText(FbViewData::DSCR, GetDescr(database));
		info->SetText(FbViewData::FILE, GetFiles(database));
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
	FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, winid, m_view.GetCode(), html).Post(m_frame);
}

wxString FbViewThread::GetDescr(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT description FROM books WHERE id=? AND description IS NOT NULL");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_view.GetCode());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow())
		return result.GetString(0);
	else return wxEmptyString;
}

wxString FbViewThread::GetFiles(wxSQLite3Database &database)
{
	FbExtractArray items(database, m_view.GetCode());

	wxString html;

	for (size_t i = 0; i<items.Count(); i++) {
		html << wxString::Format(wxT("<p>%s</p>"), items[i].ErrorName().c_str());
	}

	return html;
}


