#ifndef __LIMITEDTEXTCTRL_H__
#define __LIMITEDTEXTCTRL_H__

#include <wx/textctrl.h>

#define LTC_ROW_LIMIT 128

class LimitedTextCtrl: public wxTextCtrl
{
	public:
		virtual void AppendText(const wxString& text);
};

#endif // __LIMITEDTEXTCTRL_H__
