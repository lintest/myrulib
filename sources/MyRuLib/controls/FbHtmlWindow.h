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
			long style = wxSUNKEN_BORDER
		);
		bool Create(
			wxWindow *parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxSUNKEN_BORDER
		);
		virtual bool SetPage(const wxString& source);
		void UnselectALL() { wxDELETE(m_selection); }
	protected:
		virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const;
	private:
		DECLARE_CLASS(FbHtmlWindow)
};

#endif // __FBHTMLWINDOW_H__
