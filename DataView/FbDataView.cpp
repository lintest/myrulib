#include "FbDataView.h"
#include "FbModelData.h"

BEGIN_EVENT_TABLE( FbDataViewCtrl, wxDataViewCtrl )
    EVT_KEY_DOWN(FbDataViewCtrl::OnKeyUp)
    EVT_KEY_UP(FbDataViewCtrl::OnKeyUp)
    EVT_CHAR(FbDataViewCtrl::OnKeyUp)
	EVT_SIZE(FbDataViewCtrl::OnSize)
END_EVENT_TABLE()

void FbDataViewCtrl::OnSize(wxSizeEvent& event)
{
    Resize();
    event.Skip();
}

void FbDataViewCtrl::Resize()
{
    int width = GetClientSize().GetWidth();
    size_t count = GetColumnCount();
    for (int i = 1; i<count; i++) {
        width -= this->GetColumn(i)->GetWidth();
    }
    if (width < 200) width = 200;

    if (count) {
        GetColumn(0)->SetWidth(width);
        OnColumnChange(0);
    }
}

void FbDataViewCtrl::OnKeyUp(wxKeyEvent& event)
{
    wxLogMessage(wxT("MyComboBox::OnKeyUp"));
    event.Skip();
}
