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
	if (result.NextRow()) {
		FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_AUTH_PREVIEW, m_view.GetCode(), result.GetString(0)).Post(m_frame);
	}
}

void FbBookThread::OpenBook()
{
	wxString sql = wxT("SELECT description FROM books WHERE id=? AND description IS NOT NULL");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_view.GetCode());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) {
		FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_BOOK_PREVIEW, m_view.GetCode(), result.GetString(0)).Post(m_frame);
	}
}

void FbBookThread::OpenNone()
{
}
