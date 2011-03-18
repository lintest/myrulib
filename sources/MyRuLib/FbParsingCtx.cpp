#include "FbParsingCtx.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbParsingContext
//-----------------------------------------------------------------------------

void FbParsingContext::Inc(const wxString &tag)
{
/*
	if (level == 1) {
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

	size_t count = m_tags.Count();
	if (count > level) m_tags.RemoveAt(level, count - level);
	m_tags.SetCount(level);
	m_tags.Add(tag);
	m_name = tag;
*/
}

void FbParsingContext::Dec(const wxString &tag)
{
/*
	if (level == 1) {
		m_section = fbsNone;
	}
	size_t count = m_tags.Count();
	while (count > level) {
		m_tags.RemoveAt(count - 1);
		count = m_tags.Count();
	}
*/
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
