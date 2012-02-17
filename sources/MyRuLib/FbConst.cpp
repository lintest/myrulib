#include "FbConst.h"
#include <wx/utils.h>

#include "config.h"

wxString MyRuLib::ProgramName()
{
	return (wxString)wxT(PACKAGE_NAME)
#ifdef FB_INCLUDE_READER
		+ wxT("CR")
#endif
	+ wxT(' ') + wxT(PACKAGE_VERSION);
}

wxString MyRuLib::HomePage()
{
	return wxT(PACKAGE_URL);
}

wxString MyRuLib::MailAddr()
{
	return wxT(PACKAGE_BUGREPORT);
}

wxString MyRuLib::UserAgent()
{
	return (wxString)wxT(PACKAGE_NAME) + wxT('/') + wxT(PACKAGE_VERSION) + wxT(' ') + ::wxGetOsDescription();
}

static wxString GetRussianAlphabet()
{
	wxString result;
	for (wxChar ch = 0x0410; ch <= (wxChar)0x042F ; ch++) {
		if (ch == (wxChar)0x042A) continue;
		if (ch == (wxChar)0x042C) continue;
		result += ch;
	}
	return result;
}

const wxString alphabetRu = GetRussianAlphabet();
const wxString alphabetEn = wxT("#ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const wxString strAlphabet = alphabetRu + alphabetEn;
const wxString strRusJE = (wxChar)0x0415;
const wxString strRusJO = (wxChar)0x0416;

wxString GetRatingText(int index)
{
	const wxChar * text[] = {
		_("Clear"),
		wxT("*"),
		wxT("* *"),
		wxT("* * *"),
		wxT("* * * *"),
		wxT("* * * * *"),
	};

	if (0 <= index && index <=5)
		return text[index];
	else
		return wxEmptyString;
};

static wxString CreateMessage(const wxString &info, const wxString &text)
{
	return info + wxT(": ") + text;
}

void FbLogMessage(const wxString &info, const wxString &text)
{
	wxLogMessage(CreateMessage(info, text));
}

void FbLogError(const wxString &info, const wxString &text)
{
	wxLogError(CreateMessage(info, text));
}

void FbLogWarning(const wxString &info, const wxString &text)
{
	wxLogWarning(CreateMessage(info, text));
}

void FbMessageBox(const wxString &info, const wxString &text)
{
	wxMessageBox(CreateMessage(info, text));
}

