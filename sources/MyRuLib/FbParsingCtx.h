#ifndef __FBPARSINGCTX_H__
#define __FBPARSINGCTX_H__

#include <wx/wx.h>

enum FbSectionEnum {
	fbsNone,
	fbsBody,
	fbsDescr,
	fbsBinary,
};

class FbParsingContext: public wxObject
{
	public:
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