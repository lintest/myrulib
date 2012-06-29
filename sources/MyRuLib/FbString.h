#ifndef __FBSTRING_H__
#define __FBSTRING_H__

#include <wx/wx.h>

class FbString : public wxString
{
public:
	FbString(const wxChar * psz) : wxString(psz) {}

	FbString(const wxString & str) : wxString(str) {}

	FbString(const char * psz) : wxString(psz, wxConvUTF8) {}

	FbString Translate() const { return wxGetTranslation(*this); }

	wxString & Shorten(int length = 0x20) {
		if (Len() <= (size_t)length) return *this;
		Truncate(length);
		int pos = Find(wxT(' '), true);
		if (pos >= length / 2) Truncate(pos);
		*this += wxChar(0x2026);
		return *this;
	}
};

#define fbT(str) (FbString(str))

#endif // __FBSTRING_H__
