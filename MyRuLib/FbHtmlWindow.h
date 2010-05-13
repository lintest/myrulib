#ifndef __FBHTMLWINDOW_H__
#define __FBHTMLWINDOW_H__

#include <wx/html/htmlwin.h>

class FbHtmlWindow: public wxHtmlWindow
{
	public:
		FbHtmlWindow() {};
		FbHtmlWindow(
			wxWindow *parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxSUNKEN_BORDER,
			const wxString& name = wxT("fbhtmlvindow")
		);
		bool Create(
			wxWindow *parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxSUNKEN_BORDER,
			const wxString& name = wxT("fbhtmlvindow")
		);
		virtual bool SetPage(const wxString& source);
	protected:
		virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const;
		DECLARE_CLASS(FbHtmlWindow)
};

#endif // __FBHTMLWINDOW_H__
