#include "FbConst.h"
#include <wx/utils.h>

#include "../version.inc"

#define TXT(text) wxT(wxSTRINGIZE(text))

wxString MyRuLib::ProgramName()
{
	return (wxString)TXT(PROGRAM_NAME) + wxT(' ') + TXT(VERSION_MAJOR) + wxT('.') + TXT(VERSION_MINOR);
}

wxString MyRuLib::ProgramInfo()
{
	return ProgramName() + wxT('.') + TXT(VERSION_BUILD);
	return TXT(PROGRAM_HOMEPAGE);
}

wxString MyRuLib::HomePage()
{
	return wxT(PROGRAM_HOMEPAGE);
}

wxString MyRuLib::MailAddr()
{
	return wxT(PROGRAM_MAILADDR);
}

wxString MyRuLib::UserAgent()
{
	return (wxString)TXT(PROGRAM_NAME) + wxT(' ') + TXT(VERSION_MAJOR) + wxT('.') + TXT(VERSION_MINOR) + TXT(VERSION_BUILD) + wxT('/') + ::wxGetOsDescription(); 
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

const wxString strUpdateAuthorCount = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id)");
const wxString strUpdateSequenCount = wxT("UPDATE sequences SET number=(SELECT COUNT(DISTINCT id_book) FROM bookseq WHERE bookseq.id_seq=sequences.id)");

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

