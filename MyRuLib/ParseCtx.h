#ifndef __PARSECTX_H__
#define __PARSECTX_H__

#include <expat.h>
#include <wx/wx.h>

class ParsingContext
{
public:
	ParsingContext();
	virtual ~ParsingContext();
	wxString Path(size_t count);
	wxString Path() { return m_path; };
	void AppendTag(wxString &tag);
	void RemoveTag(wxString &tag);
	size_t Level() {return m_tags.Count();};
	XML_Parser & GetParser() {return m_parser;};
	void Stop();
public:
	static wxString CharToString(const char *s, size_t len = wxString::npos);
	static wxString CharToLower(const char *s, size_t len = wxString::npos);
	static bool IsWhiteOnly(const wxChar *buf);
public:
	wxString encoding;
	wxString version;
private:
	wxArrayString m_tags;
	XML_Parser m_parser;
	wxString m_path;
};

#endif // __PARSECTX_H__
