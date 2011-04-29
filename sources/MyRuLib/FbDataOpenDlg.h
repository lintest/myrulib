#ifndef __FBDATAOPENDLG_H__
#define __FBDATAOPENDLG_H__

#include <wx/wx.h>
#include "FbWindow.h"
#include "controls/FbChoiceCtrl.h"

class FbThread;

class FbDataOpenDlg : public FbDialog
{
	public:
		static wxString Execute(wxWindow * parent);
		FbDataOpenDlg( wxWindow * parent);
		FbThread * CreateThread(wxEvtHandler * owner);
	private:
		enum
		{
			ID_ACTION = 1000,
			ID_FILE_TXT,
			ID_FILE_BTN,
			ID_FOLDER_TXT,
			ID_FOLDER_BTN,
			ID_DOWNLOAD,
		};
		wxString GetTitle() const;
		void UpdateFolder();
		wxString CheckExt(const wxString &filename);
		void SetDefaultNames();
	private:
		wxString GetFilename();
		wxString GetDirname();
	private:
		FbChoiceStr m_action;
		wxComboBox m_file;
		wxTextCtrl m_folder;
		wxCheckBox m_scaner;
		wxCheckBox m_import;
		wxCheckBox m_only;
	private:
		void OnActionChoise( wxCommandEvent& event );
		void OnSelectFileClick( wxCommandEvent& event );
		void OnSelectFolderClick( wxCommandEvent& event );
		void OnFileCombo( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};


#endif // __FBDATAOPENDLG_H__
