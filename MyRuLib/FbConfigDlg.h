///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FBCONFIGDLG_H__
#define __FBCONFIGDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/arrimpl.cpp>
#include "FbDatabase.h"
#include "FbWindow.h"

///////////////////////////////////////////////////////////////////////////////
/// Class FbConfigDlg
///////////////////////////////////////////////////////////////////////////////
class FbConfigDlg : private FbDialog
{
	public:
		FbConfigDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		static void Execute(wxWindow* parent);
	private:
		void Assign(bool write);
	private:
		FbCommonDatabase m_database;
		enum ID {
			ID_LIBRARY_TITLE = 1000,
			ID_LIBRARY_DIR_TXT,
			ID_LIBRARY_DIR_BTN,
			ID_WANRAIK_DIR_TXT,
			ID_WANRAIK_DIR_BTN,
			ID_LIBRARY_DESCR,
			ID_DOWNLOAD_HOST,
			ID_DOWNLOAD_USER,
			ID_DOWNLOAD_PASS,
		};
		class PanelMain: public wxPanel
		{
			public:
				PanelMain(wxWindow *parent);
		};
		class PanelInternet: public wxPanel
		{
			public:
				PanelInternet(wxWindow *parent);
		};
	private:
		void OnSelectFolderClick( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif //__FBCONFIGDLG_H__
