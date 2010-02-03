#include "FbHtmlWindow.h"
#include "FbConst.h"
#include "FbParams.h"
#include <wx/uri.h>

class FbURI: public wxURI
{
	public:
		FbURI(const wxString& uri): wxURI(uri) {};
		friend class FbHtmlWindow;
};

BEGIN_EVENT_TABLE(FbHtmlWindow, wxHtmlWindow)
	EVT_HTML_LINK_CLICKED(wxID_ANY, FbHtmlWindow::OnLinkClicked)
END_EVENT_TABLE()

FbHtmlWindow::FbHtmlWindow(wxWindow *parent, wxWindowID id)
	: wxHtmlWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
	SetHTMLBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

bool FbHtmlWindow::Create(wxWindow *parent, wxWindowID id)
{
	bool ok = wxHtmlWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	SetHTMLBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	return ok;
}


bool FbHtmlWindow::SetPage(const wxString& source)
{
	wxString colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString(wxC2S_HTML_SYNTAX);
	wxString html = wxString::Format(wxT("<html><body text=%s>%s</html>"), colour.c_str(), source.c_str());
	bool ok = wxHtmlWindow::SetPage(html);
	SetHTMLBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	return ok;
}

wxHtmlOpeningStatus FbHtmlWindow::OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const
{
	if (type != wxHTML_URL_IMAGE) return wxHTML_OPEN;

	wxString addr = url;
	FbURI uri = addr;

	if (uri.GetScheme() == wxT("http")) {
		if ( !FbParams::GetValue(FB_HTTP_IMAGES) ) {
			*redirect = wxT("memory:blank");
			return wxHTML_REDIRECT;
		}
	}

	return wxHTML_OPEN;
}

void FbHtmlWindow::OnLinkClicked(wxHtmlLinkEvent& event)
{
	wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}

