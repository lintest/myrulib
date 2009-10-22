#include "FbBookThread.h"
#include "InfoCash.h"

void FbBookThread::UpdateInfo()
{
	if (!m_id) return;
	wxThread *thread = new ShowThread(this);
	if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

//! Return specified string with the html special characters encoded.
//! Similar to PHP's htmlspecialchars() function.
wxString FbBookThread::HTMLSpecialChars( const wxString &value, const bool bSingleQuotes, const bool bDoubleQuotes )
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

