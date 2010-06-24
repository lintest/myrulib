#include "FbBookThread.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbDatabase.h"
#include "InfoCash.h"

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
//  FbBookThread
//-----------------------------------------------------------------------------

void * FbBookThread::Entry()
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

void FbBookThread::OpenAuth()
{
	wxString sql = wxT("SELECT description FROM authors WHERE id=? AND description IS NOT NULL");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_view.GetCode());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) SendHTML(ID_AUTH_PREVIEW, result.GetString(0));
}

void FbBookThread::OpenBook()
{
	int id = m_view.GetCode();
	FbCacheBook book = FbCollection::GetBookData(id);
	if (!book) return;

	wxString html = FbCollection::GetBookHTML(m_ctx, book, id);
	if (!html.IsEmpty()) {
		SendHTML(ID_BOOK_PREVIEW, html);
		return;
	}

	FbBookInfo info(id);
	wxString descr = GetDescr();
	if (!descr.IsEmpty()) {
		info.SetText(FbBookInfo::DSCR, descr);
		FbCollection::AddInfo(new FbBookInfo(info));
	}

	html = info.GetHTML(m_ctx, book);
	SendHTML(ID_BOOK_PREVIEW, html);
}

void FbBookThread::OpenNone()
{
}

void FbBookThread::SendHTML(wxWindowID winid, const wxString &html)
{
	FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, winid, m_view.GetCode(), html).Post(m_frame);
}

wxString FbBookThread::GetDescr()
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
