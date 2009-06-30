#include "ParseCtx.h"

wxString ParsingContext::Path(size_t count)
{
	size_t lastNum;
	wxString result;

	if (count>m_tags.Count())
		lastNum = m_tags.Count();
	else
		lastNum = (count ? count : m_tags.Count());

	for (size_t i=0; i<lastNum; i++)
		result += m_tags[i] + wxT("/");

	return result;
}

void ParsingContext::AppendTag(wxString &tag)
{
	m_tags.Add(tag);
}

void ParsingContext::RemoveTag(wxString &tag)
{
	for (size_t i = m_tags.Count(); i>0 ; i--) {
		if ( m_tags[i-1] == tag) {
			while (m_tags.Count()>=i) m_tags.RemoveAt(i-1);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
//  XML_Parser loading routines
//-----------------------------------------------------------------------------

// converts Expat-produced string in UTF-8 into wxString using the specified
// conv or keep in UTF-8 if conv is NULL
wxString ParsingContext::CharToString(const char *s, size_t len)
{
    return wxString(s, wxConvUTF8, len);
}

wxString ParsingContext::CharToLower(const char *s, size_t len)
{
    wxString data = wxString(s, wxConvUTF8, len);
    data.MakeLower();
    data.Trim(false).Trim(true);
    return data;
}

// returns true if the given string contains only whitespaces
bool ParsingContext::IsWhiteOnly(const wxChar *buf)
{
    for (const wxChar *c = buf; *c != wxT('\0'); c++)
        if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r'))
            return false;
    return true;
}

