#ifndef __FBDATAOPENDLG_H__
#define __FBDATAOPENDLG_H__

#include <wx/wx.h>
#include "FbWindow.h"

class FbDataOpenDlg : public FbDialog
{
	public:
		FbDataOpenDlg( wxWindow* parent);
		wxString GetFilename();
	private:
		enum
		{
			ID_FILE_TXT = 1000,
			ID_FILE_BTN,
			ID_DOWNLOAD,
		};
		wxString GetTitle() const;
		wxString CheckExt(const wxString &filename);
		void OnSelectFileClick( wxCommandEvent& event );
	private:
		wxComboBox* m_FileBox;
		wxCheckBox* m_FileCheck;
		DECLARE_EVENT_TABLE()
};


#endif // __FBDATAOPENDLG_H__
