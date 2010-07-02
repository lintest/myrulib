#include <wx/wx.h>
#include <wx/dir.h>
#include "FbThread.h"

class FbCollection;

class FbZipCatalogueThread: public FbThread
{
	public:
		FbZipCatalogueThread(FbCollection & owner, const wxString &dirname)
			: m_owner(owner), m_dirname(dirname) {}
		virtual void * Entry();
	private:
		FbCollection & m_owner;
		wxString m_dirname;
};
