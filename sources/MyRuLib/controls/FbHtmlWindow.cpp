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

IMPLEMENT_CLASS(FbHtmlWindow, wxHtmlWindow)

FbHtmlWindow::FbHtmlWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxHtmlWindow(parent, id, pos, size, style)
{
	SetHTMLBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

bool FbHtmlWindow::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	bool ok = wxHtmlWindow::Create(parent, id, pos, size, style);
	SetHTMLBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	return ok;
}

bool FbHtmlWindow::SetPage(const wxString& source)
{
	wxString colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString(wxC2S_HTML_SYNTAX);
	wxString html = wxString::Format(wxT("<html><body text=%s>%s</html>"), colour.c_str(), source.c_str());
	bool ok = false; try { ok = wxHtmlWindow::SetPage(html); } catch (...) {}
	SetHTMLBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	return ok;
}

wxHtmlOpeningStatus FbHtmlWindow::OnOpeningURL(wxHtmlURLType type, const wxString& url, wxString * redirect) const
{
	if (type != wxHTML_URL_IMAGE) return wxHTML_OPEN;

	FbURI uri = url;
	if (uri.GetScheme() == wxT("http")) {
		if ( !FbParams::GetInt(FB_HTTP_IMAGES) ) {
			*redirect = wxT("memory:blank");
			return wxHTML_REDIRECT;
		}
	}

	return wxHTML_OPEN;
}
