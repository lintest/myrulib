#include "FbViewThread.h"
#include "FbViewReader.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbColumns.h"
#include "FbDatabase.h"
#include "InfoCash.h"
#include "ZipReader.h"

//-----------------------------------------------------------------------------
//  FbBookThreadBase
//-----------------------------------------------------------------------------

void FbBookThreadBase::UpdateInfo()
{
	if (!m_id) return;
	wxThread *thread = new ShowThread(this);
	if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

//! Return specified string with the html special characters encoded.
//! Similar to PHP's htmlspecialchars() function.
wxString FbBookThreadBase::HTMLSpecialChars( const wxString &value, const bool bSingleQuotes, const bool bDoubleQuotes )
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
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
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
	info->SetText(FbViewData::DSCR, GetDescr());
	SendHTML(*info);

	if (IsClosed()) { delete info; return; }

	ZipReader zip(id, false, true);
	if (zip.IsOK()) FbViewReader(*this, *info).Load(zip.GetZip());
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

wxString FbViewThread::GetDescr()
{
	wxString sql = wxT("SELECT description FROM books WHERE id=? AND description IS NOT NULL");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_view.GetCode());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow())
		return result.GetString(0);
	else return wxEmptyString;
}

