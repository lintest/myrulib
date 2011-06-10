#ifndef __FBVIEWERDLG_H__
#define __FBVIEWERDLG_H__

#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/combo.h>

#include "FbWindow.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FbViewerDlg
///////////////////////////////////////////////////////////////////////////////
class FbViewerDlg : public FbDialog
{
	public:
		static bool Execute( wxWindow* parent, const wxString& type, wxString &value, bool relative = false);
		FbViewerDlg( wxWindow* parent, const wxString& type, const wxString& value, bool relative = false);
		wxString GetValue();
	protected:
		enum
		{
			ID_FILENAME = 1000,
		};
		wxComboCtrl * m_filename;
		bool m_relative;
	private:
		void OnTextEnter( wxCommandEvent& event );
		void OnBtnClick( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif //__FBVIEWERDLG_H__
