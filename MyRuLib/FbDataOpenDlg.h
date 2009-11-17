#ifndef __FBDATAOPENDLG_H__
#define __FBDATAOPENDLG_H__

#include <wx/wx.h>
#include "FbWindow.h"

class FbDataOpenDlg : public FbDialog
{
	public:
		FbDataOpenDlg( wxWindow* parent, const wxString& title, bool bMustWxists);
		wxString GetFilename();
	private:
		enum
		{
			ID_FILE_TXT = 1000,
			ID_FILE_BTN,
			ID_DOWNLOAD,
		};
		wxString GetTitle(const wxString& title) const;
		void OnSelectFileClick( wxCommandEvent& event );
	private:
		wxComboBox* m_FileBox;
		wxCheckBox* m_FileCheck;
		DECLARE_EVENT_TABLE()
		bool m_MustExitst;
};


#endif // __FBDATAOPENDLG_H__
