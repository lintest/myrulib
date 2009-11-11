#ifndef __FBWINDOW_H__
#define __FBWINDOW_H__

#include <wx/wx.h>
#include <wx/dialog.h>

class FbDialog: public wxDialog
{
	public:
		FbDialog() {};
		FbDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		bool Create( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
};

#endif // __FBWINDOW_H__
