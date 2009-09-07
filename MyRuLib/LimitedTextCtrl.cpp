#include "LimitedTextCtrl.h"

void LimitedTextCtrl::AppendText(const wxString& text)
{
    int pos = GetNumberOfLines();
    int delta = wxString(wxT("\n")).Length();
    if (pos > LTC_ROW_LIMIT) {
        int len = 0;
        for (int i=0; i < (pos-LTC_ROW_LIMIT); i++) {
            len += GetLineLength(i) + delta;
        }
        Remove(0, len);
    }
    wxTextCtrl::AppendText(text);
}

