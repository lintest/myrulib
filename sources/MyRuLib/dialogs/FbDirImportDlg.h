#ifndef __FBDIRIMPORTDLG_H__
#define __FBDIRIMPORTDLG_H__

#include <wx/wx.h>
#include "FbWindow.h"
#include "controls/FbComboBox.h"

class FbThread;

class FbDirImportDlg : public FbDialog
{
	public:
		static bool Execute(wxWindow * parent);
		FbDirImportDlg( wxWindow * parent, const wxString &title);
		FbThread * CreateThread(wxEvtHandler * owner);
	private:
		enum
		{
			ID_FOLDER = 1000,
			ID_ONLY_NEW,
			ID_ONLY_MD5,
		};
		wxString GetDirname();
	private:
		FbCustomCombo m_folder;
		wxCheckBox m_only_new;
		wxCheckBox m_only_md5;
	private:
		void OnSelectFolderClick( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};


#endif // __FBDIRIMPORTDLG_H__
