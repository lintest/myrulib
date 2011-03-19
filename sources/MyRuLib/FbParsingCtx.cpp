#include "FbParsingCtx.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbParsingContext
//-----------------------------------------------------------------------------

wxString FbParsingContext::CharToString(const FAXPP_Text *text)
{
	return wxString((char*)text->ptr, wxConvUTF8, text->len);
}

wxString FbParsingContext::CharToLower(const FAXPP_Text *text)
{
	wxString data = wxString((char*)text->ptr, wxConvUTF8, text->len);
	data.MakeLower();
	data.Trim(false).Trim(true);
	return data;
}

// returns true if the given string contains only whitespaces
bool FbParsingContext::IsWhiteOnly(const FAXPP_Text *text)
{
	char * buffer = (char*) text->ptr;
	char * buffer_end = buffer + text->len;
	for (const char * c = buffer; c < buffer_end; c++) {
		if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r')) return false;
	}
	return true;
}

void FbParsingContext::Inc(const wxString &tag)
{
	if (m_tags.Count() == 1) {
		if (tag == wxT("body")) {
			m_section = fbsBody;
		} else if (tag == wxT("description")) {
			m_section = fbsDescr;
		} else if (tag == wxT("binary")) {
			m_section = fbsBinary;
		} else {
			m_section = fbsNone;
		}
	}
	m_tags.Add(tag);
	m_name = tag;
}

void FbParsingContext::Dec(const wxString &tag)
{
	size_t count = m_tags.Count();
	size_t index = count;
	while (index > 0) {
		index--;
		if (m_tags[index] == tag) {
			m_tags.RemoveAt(index, count - index);
			break;
		}
	}
	if (m_tags.Count() == 1) m_section = fbsNone;
}

bool FbParsingContext::operator == (const wxString & tags)
{
	wxStringTokenizer tkz(tags, wxT("/"), wxTOKEN_STRTOK);
	size_t index = 0;
	size_t count = m_tags.Count();
	while (tkz.HasMoreTokens()) {
		if (index >= count) return false;
		wxString token = tkz.GetNextToken();
		if (m_tags[index] != token) return false;
		index++;
	}

	return (count == index);
}

bool FbParsingContext::operator >= (const wxString & tags)
{
	wxStringTokenizer tkz(tags, wxT("/"), wxTOKEN_STRTOK);
	size_t index = 0;
	size_t count = m_tags.Count();
	while (tkz.HasMoreTokens()) {
		if (index >= count) return false;
		wxString token = tkz.GetNextToken();
		if (m_tags[index] != token) return false;
		index++;
	}
	return (count >= index);
}

bool FbParsingContext::operator > (const wxString & tags)
{
	wxStringTokenizer tkz(tags, wxT("/"), wxTOKEN_STRTOK);
	size_t index = 0;
	size_t count = m_tags.Count();
	while (tkz.HasMoreTokens()) {
		if (index >= count) return false;
		wxString token = tkz.GetNextToken();
		if (m_tags[index] != token) return false;
		index++;
	}

	return (count > index);
}

wxString FbParsingContext::Path() const
{
	wxString result;
	size_t count = m_tags.Count();
	for (size_t i = 0; i < count; i++) {
		result << wxT("/") << m_tags[i];
	}
	return result;
}
