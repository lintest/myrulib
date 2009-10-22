#include "LimitedTextCtrl.h"

void LimitedTextCtrl::AppendText(const wxString& text)
{
	int pos = GetNumberOfLines();
	if (pos > LTC_ROW_LIMIT) {
		int len = 0;
		for (int i=0; i < (pos-LTC_ROW_LIMIT); i++) {
			len += GetLineLength(i) + 1;
		}
		Remove(0, len);
	}
	wxTextCtrl::AppendText(text);
}

