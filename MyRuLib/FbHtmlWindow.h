#ifndef __FBHTMLWINDOW_H__
#define __FBHTMLWINDOW_H__

#include <wx/html/htmlwin.h>

class FbHtmlWindow: public wxHtmlWindow
{
	public:
		FbHtmlWindow() {};
		FbHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY);
		bool Create(wxWindow *parent, wxWindowID id = wxID_ANY);
		virtual bool SetPage(const wxString& source);
	protected:
		virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const;
	private:
		void OnLinkClicked(wxHtmlLinkEvent& event);
		DECLARE_EVENT_TABLE();
};

#endif // __FBHTMLWINDOW_H__
