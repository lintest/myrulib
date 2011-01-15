#include "FbConst.h"

#include "../version.inc"

#define TXT(text) wxT(wxSTRINGIZE(text))

const wxString strProgramName = (wxString) TXT(PROGRAM_NAME) + wxT(' ') + TXT(VERSION_MAJOR) + wxT('.') + TXT(VERSION_MINOR);
const wxString strProgramInfo = strProgramName + wxT('.') + TXT(VERSION_BUILD);
const wxString strHomePage = wxT("http://myrulib.lintest.ru");
const wxString strMailAddr = wxT("mail@lintest.ru");
const wxString strVersionInfo = strHomePage +  wxT("\n") + strMailAddr;

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
const wxString strNobody = _("(no Author)");
const wxString strRusJE = (wxChar)0x0415;
const wxString strRusJO = (wxChar)0x0416;
const wxString strOtherSequence = wxT("(Misc.)");

const wxString strUpdateAuthorCount = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id)");
const wxString strUpdateSequenCount = wxT("UPDATE sequences SET number=(SELECT COUNT(DISTINCT id_book) FROM bookseq WHERE bookseq.id_seq=sequences.id)");

const int ciMaxImageWidth = 200;

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

