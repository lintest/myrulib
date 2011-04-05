#ifndef __FBPARSINGCTX_H__
#define __FBPARSINGCTX_H__

#include <wx/wx.h>

extern "C" {
	#undef NO_ERROR
	#include <faxpp/parser.h>
	#include <faxpp/error.h>
	#define NO_ERROR 0
}

enum FbSectionEnum {
	fbsNone,
	fbsBody,
	fbsDescr,
	fbsBinary,
};

class FbParsingContext: public wxObject
{
	public:
		static wxString CharToString(const FAXPP_Text * text);
		static wxString CharToLower(const FAXPP_Text * text);
		static bool IsWhiteOnly(const FAXPP_Text *text);
		FbParsingContext(): m_section(fbsNone) {}
		void Inc(const wxString &tag);
		void Dec(const wxString &tag);
		bool operator == (const wxString & tags);
		bool operator >= (const wxString & tags);
		bool operator > (const wxString & tags);
		FbSectionEnum Section() { return m_section; }
		wxString Path() const;
	private:
		FbSectionEnum m_section;
		wxArrayString m_tags;
		wxString m_name;
		wxString m_text;
};

#endif // __FBPARSINGCTX_H__
