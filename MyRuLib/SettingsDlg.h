///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SettingsDlg__
#define __SettingsDlg__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/arrimpl.cpp>
#include "FbDatabase.h"

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlg
///////////////////////////////////////////////////////////////////////////////
class SettingsDlg : private wxDialog
{
DECLARE_EVENT_TABLE()
private:
    enum ID {
        ID_LIBRARY_TITLE = 1000,
        ID_LIBRARY_DIR_TXT,
        ID_LIBRARY_DIR_BTN,
        ID_WANRAIK_DIR_TXT,
        ID_WANRAIK_DIR_BTN,
        ID_EXTERNAL_TXT,
        ID_EXTERNAL_BTN,
        ID_TRANSLIT_FOLDER,
        ID_TRANSLIT_FILE,
        ID_FOLDER_FORMAT,
        ID_FILE_FORMAT,
        ID_USE_SYMLINKS,
        ID_USE_PROXY,
        ID_PROXY_ADDR,
        ID_PROXY_PORT,
        ID_PROXY_NAME,
        ID_PROXY_PASS,
        ID_TYPELIST,
        ID_APPEND_TYPE,
        ID_MODIFY_TYPE,
        ID_DELETE_TYPE,
    };
public:
    SettingsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
    virtual ~SettingsDlg();
    static void Execute(wxWindow* parent);
private:
    void Assign(bool write);
    void OnSelectFolderClick( wxCommandEvent& event );
    void OnAppendType( wxCommandEvent& event );
    void OnModifyType( wxCommandEvent& event );
    void OnDeleteType( wxCommandEvent& event );
	void OnTypelistActivated( wxListEvent & event );
	void SelectApplication();
	void FillTypelist();
	void SaveTypelist();
private:
	wxListCtrl* m_typelist;
	wxArrayString m_commands;
    FbCommonDatabase m_database;
    wxArrayString m_deleted;
};

#endif //__SettingsDlg__
