#include "FbHtmlWindow.h"
#include "FbConst.h"
#include "FbParams.h"
#include <wx/uri.h>
#include <wx/clipbrd.h>

class FbURI: public wxURI
{
	public:
		FbURI(const wxString& uri): wxURI(uri) {};
		friend class FbHtmlWindow;
};

IMPLEMENT_CLASS(FbHtmlWindow, wxHtmlWindow)

BEGIN_EVENT_TABLE(FbHtmlWindow, wxHtmlWindow)
	EVT_MENU(wxID_COPY, FbHtmlWindow::OnCopy)
	EVT_MENU(wxID_SELECTALL, FbHtmlWindow::OnSelect)
	EVT_MENU(ID_UNSELECTALL, FbHtmlWindow::OnUnselect)
	EVT_UPDATE_UI(wxID_CUT, FbHtmlWindow::OnDisableUI)
	EVT_UPDATE_UI(wxID_COPY, FbHtmlWindow::OnEnableUI)
	EVT_UPDATE_UI(wxID_PASTE, FbHtmlWindow::OnDisableUI)
	EVT_UPDATE_UI(wxID_SELECTALL, FbHtmlWindow::OnEnableUI)
	EVT_UPDATE_UI(ID_UNSELECTALL, FbHtmlWindow::OnEnableUI)
END_EVENT_TABLE()

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
	if (type == wxHTML_URL_IMAGE && FbURI(url).GetScheme() == wxT("http")) {
		if ( !FbParams(FB_HTTP_IMAGES) ) {
			*redirect = wxT("memory:blank");
			return wxHTML_REDIRECT;
		}
	}
	return wxHTML_OPEN;
}

void FbHtmlWindow::UpdateFont(bool refresh)
{
	wxFont font = FbParams(FB_FONT_HTML);

	int fontsizes[7] = {6, 8, 9, 10, 12, 16, 18};
	int size = font.GetPointSize();

	fontsizes[0] = size - 2;
	fontsizes[1] = size;
	fontsizes[2] = size + 1;
	fontsizes[3] = size + 2;
	fontsizes[4] = size + 4;
	fontsizes[5] = size + 8;
	fontsizes[6] = size + 10;

	SetFonts(font.GetFaceName(), font.GetFaceName(), fontsizes);
}

void FbHtmlWindow::OnCopy(wxCommandEvent& event)
{
	wxString text = SelectionToText();
	if (text.IsEmpty()) return;

	wxClipboardLocker locker;
	if (!locker) return;
	wxTheClipboard->SetData( new wxTextDataObject(text) );
}

void FbHtmlWindow::OnSelect(wxCommandEvent& event)
{
	SelectAll();
}

void FbHtmlWindow::OnUnselect(wxCommandEvent& event)
{
	UnselectALL();
}

void FbHtmlWindow::OnEnableUI(wxUpdateUIEvent & event)
{
	event.Enable(true);
}

void FbHtmlWindow::OnDisableUI(wxUpdateUIEvent & event)
{
	event.Enable(false);
}
