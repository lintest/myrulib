#ifndef __FBSEARCHCTRL_H__
#define __FBSEARCHCTRL_H__

#include <wx/wx.h>
#include <wx/srchctrl.h>

class FbSearchCtrl: public wxSearchCtrl
{
    public:
        FbSearchCtrl() {};

        virtual ~FbSearchCtrl() {};

        FbSearchCtrl(wxWindow *parent, wxWindowID id,
                   const wxString& value = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxSearchCtrlNameStr);

        bool Create(wxWindow *parent, wxWindowID id,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxSearchCtrlNameStr);

	private:
		void OnChar(wxKeyEvent &event);
        DECLARE_EVENT_TABLE()
};

#endif // __FBSEARCHCTRL_H__
