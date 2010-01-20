#ifndef __FBHTMLWINDOW_H__
#define __FBHTMLWINDOW_H__

#include <wx/html/htmlwin.h>

class FbHtmlWindow: public wxHtmlWindow
{
	public:
		FbHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY);
	protected:
		virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const;
	private:
		void OnLinkClicked(wxHtmlLinkEvent& event);
		DECLARE_EVENT_TABLE();
};

#endif // __FBHTMLWINDOW_H__
