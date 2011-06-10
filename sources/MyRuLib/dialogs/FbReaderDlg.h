#ifndef __FBREADERDLG_H__
#define __FBREADERDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/fontpicker.h>
#include <wx/gbsizer.h>
#include <wx/clrpicker.h>
#include "FbWindow.h"
#include "FbDatabase.h"

#ifdef FB_INCLUDE_READER

class FbReaderDlg : private FbDialog
{
	public:
		FbReaderDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		virtual ~FbReaderDlg();
		static bool Execute(wxWindow* parent);
	private:
		enum ID {
			ID_FONT_DEFAULT = 1000,
			ID_FONT_COLOUR,
			ID_BACK_COLOUR,
			ID_FONT_HEADER,
		};
		class PanelMain: public wxPanel
		{
			public:
				PanelMain(wxWindow *parent);
		};
		class PanelPage: public wxPanel
		{
			public:
				PanelPage(wxWindow *parent);
		};
	private:
		void Assign(bool write);
		DECLARE_EVENT_TABLE()
};

#endif // FB_INCLUDE_READER

#endif // __FBREADERDLG_H__
