#ifndef __FBWINDOW_H__
#define __FBWINDOW_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>

class FbDialog: public wxDialog
{
	public:
		FbDialog() {};
		FbDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		bool Create( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
};

class FbAuiMDIChildFrame : public wxAuiMDIChildFrame
{
	public:
		bool Create(wxAuiMDIParentFrame *parent,
					wxWindowID winid,
					const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxDEFAULT_FRAME_STYLE,
					const wxString& name = wxFrameNameStr);
		virtual void UpdateFonts(bool refresh = true) {};
		static void UpdateFont(wxHtmlWindow * html, bool refresh);
		virtual void ShowFullScreen(bool show) {};
};

#endif // __FBWINDOW_H__
