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
			ID_FOLDER_TXT,
			ID_FOLDER_BTN,
			ID_DOWNLOAD,
		};
		wxString GetTitle() const;
		void UpdateFolder();
		wxString CheckExt(const wxString &filename);
	private:
		wxComboBox m_file;
		wxTextCtrl m_folder;
		wxCheckBox m_zips;
		wxCheckBox m_scan;
	private:
		void OnSelectFileClick( wxCommandEvent& event );
		void OnSelectFolderClick( wxCommandEvent& event );
		void OnFileCombo( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};


#endif // __FBDATAOPENDLG_H__
