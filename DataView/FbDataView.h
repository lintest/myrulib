#ifndef __FBDATAVIEW_H__
#define __FBDATAVIEW_H__

#include <wx/dataview.h>

class FbDataViewCtrl  : public wxDataViewCtrl
{
    public:
        FbDataViewCtrl() {}

        FbDataViewCtrl( wxWindow *parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = 0,
               const wxValidator& validator = wxDefaultValidator )
            : wxDataViewCtrl(parent, id, pos, size, style, validator) {}

        bool Create(wxWindow *parent, wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator )
        {
            return wxDataViewCtrl::Create(parent, id, pos, size, style, validator);
        }

        virtual ~FbDataViewCtrl () {};

        void Resize();

    private:
        void FbDataViewCtrl::OnSize(wxSizeEvent& event);
        DECLARE_EVENT_TABLE()
};

#endif // __FBDATAVIEW_H__
