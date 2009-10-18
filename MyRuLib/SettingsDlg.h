///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SettingsDlg__
#define __SettingsDlg__

#include <wx/wx.h>
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
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlg
///////////////////////////////////////////////////////////////////////////////
class SettingsDlg : private wxDialog
{
DECLARE_EVENT_TABLE()
private:
    enum ID {
        ID_LIBRARY_TITLE = 1000,
        ID_FB2_PROGRAM_TXT,
        ID_FB2_PROGRAM_BTN,
        ID_LIBRARY_DIR_TXT,
        ID_LIBRARY_DIR_BTN,
        ID_DOWNLOAD_DIR_TXT,
        ID_DOWNLOAD_DIR_BTN,
        ID_EXTRACT_DIR_TXT,
        ID_EXTRACT_DIR_BTN,
        ID_EXTRACT_DELETE,
        ID_EXTERNAL_TXT,
        ID_EXTERNAL_BTN,
        ID_TRANSLIT_FOLDER,
        ID_TRANSLIT_FILE,
        ID_FOLDER_FORMAT,
        ID_FILE_FORMAT,
        ID_USE_PROXY,
        ID_PROXY_ADDR,
        ID_PROXY_PORT,
        ID_PROXY_NAME,
        ID_PROXY_PASS,
    };
private:
    void Assign(bool write);
    void OnSelectFileClick( wxCommandEvent& event );
    void OnSelectFolderClick( wxCommandEvent& event );
public:
    SettingsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
    ~SettingsDlg();
    static void Execute(wxWindow* parent);
};

#endif //__SettingsDlg__
