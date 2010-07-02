#include <wx/wx.h>
#include <wx/dir.h>
#include "FbThread.h"

class FbZipCatalogueThread: public FbThread
{
	public:
		FbZipCatalogueThread(const wxString &dirname)
			: m_dirname(dirname) {}
		virtual void * Entry();
	private:
		wxString m_dirname;
};
