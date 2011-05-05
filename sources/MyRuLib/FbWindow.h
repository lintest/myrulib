#ifndef __FBWINDOW_H__
#define __FBWINDOW_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>

class FbDialog
	: public wxDialog
{
	public:
		FbDialog()
			{}

		FbDialog(
			wxWindow* parent,
			wxWindowID id = wxID_ANY,
			const wxString& title = wxEmptyString,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER,
			const wxString& name = wxFrameNameStr
		);

		bool Create(
			wxWindow* parent,
			wxWindowID id = wxID_ANY,
			const wxString& title = wxEmptyString,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER,
			const wxString& name = wxFrameNameStr
		);
	protected:
		void Assign(long winid, int param, bool write);
};

#endif // __FBWINDOW_H__
