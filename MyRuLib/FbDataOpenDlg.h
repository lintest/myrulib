#ifndef __FBDATAOPENDLG_H__
#define __FBDATAOPENDLG_H__

#include <wx/wx.h>

class FbDataOpenDlg : public wxDialog
{
	public:
		FbDataOpenDlg( wxWindow* parent, wxWindowID id = wxID_ANY);
		virtual wxString GetTitle() const;
	private:
		enum
		{
			ID_FILE_TXT = 1000,
			ID_FILE_BTN,
			ID_DOWNLOAD,
		};
		void OnSelectFileClick( wxCommandEvent& event );
	private:
		wxComboBox* m_FileBox;
		wxCheckBox* m_FileCheck;
		DECLARE_EVENT_TABLE()
};


#endif // __FBDATAOPENDLG_H__
