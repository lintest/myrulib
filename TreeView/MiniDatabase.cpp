#include "FbDatabase.h"

const wxString COLON = wxT(": ");

wxString Lower(const wxString & input)
{
#if defined(__WIN32__)
	int len = input.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, input.c_str());
	CharLower(buf);
	wxString output = buf;
	delete [] buf;
	return output;
#else
	return input.Lower();
#endif
}

void FbSearchFunction::Decompose(const wxString &text, wxArrayString &list)
{
	wxString str = Lower(text);
	int i = wxNOT_FOUND;
	do {
		str.Trim(false);
		i = str.find(wxT(' '));
		if (i == wxNOT_FOUND) break;
		list.Add( str.Left(i) );
		str = str.Mid(i);
	} while (true);
	str.Trim(true);
	if (!str.IsEmpty()) list.Add(str);
}

FbSearchFunction::FbSearchFunction(const wxString & input)
{
	Decompose(input, m_masks);
	wxString log = _("Search template") + COLON;
	size_t count = m_masks.Count();
	for (size_t i=0; i<count; i++) {
		log += wxString::Format(wxT("<%s> "), m_masks[i].c_str());
	}
	wxLogInfo(log);
}

void FbSearchFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount != 1) {
		ctx.SetResultError(wxString::Format(wxT("SEARCH called with wrong number of arguments: %d."), argCount));
		return;
	}
	wxString text = Lower(ctx.GetString(0));

	wxArrayString words;
	Decompose(text, words);

	size_t maskCount = m_masks.Count();
	size_t wordCount = words.Count();

	for (size_t i=0; i<maskCount; i++) {
		bool bNotFound = true;
		wxString mask = m_masks[i] + wxT("*");
		for (size_t j=0; j<wordCount; j++) {
			if ( words[j].Matches(mask) ) {
				bNotFound = false;
				break;
			}
		}
		if (bNotFound) {
			ctx.SetResult(false);
			return;
		}
	}
	ctx.SetResult(true);
}

bool FbSearchFunction::IsFullText(const wxString &text)
{
	return ( text.Find(wxT("*")) == wxNOT_FOUND ) && ( text.Find(wxT("?")) == wxNOT_FOUND );
}

wxString FbSearchFunction::AddAsterisk(const wxString &text)
{
	wxString str = Lower(text);
	wxString result;
	int i = wxNOT_FOUND;
	do {
		str.Trim(false);
		i = str.find(wxT(' '));
		if (i == wxNOT_FOUND) break;
		result += str.Left(i) + wxT("* ");
		str = str.Mid(i);
	} while (true);
	str.Trim(true);
	if (!str.IsEmpty()) result += str.Left(i) + wxT("*");
	return result;
}

