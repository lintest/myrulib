#include "FbConst.h"

#include "../version.inc"

#define __TXT(text) #text
#define TXT(text) __TXT(text)

const wxString strProgramName = (wxString)wxT("MyRuLib ") + wxT(TXT(VERSION_MAJOR)) + wxT(".") + wxT(TXT(VERSION_MINOR));
const wxString strProgramInfo = strProgramName + wxT('.') + wxT(TXT(VERSION_BUILD));
const wxString strHomePage = wxT("http://myrulib.lintest.ru");
const wxString strMailAddr = wxT("mail@lintest.ru");
const wxString strVersionInfo = strHomePage +  wxT("\n") + strMailAddr;

static wxString GetRussianAlphabet()
{
	wxString result;
	for (wxChar ch = 0x0410; ch <= (wxChar)0x042F ; ch++) {
		if (ch == (wxChar)0x0416) continue;
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
const wxString strRusJE = wxT("е");
const wxString strRusJO = wxT("ё");
const wxString strOtherSequence = wxT("(Misc.)");
const wxString strBookNotFound = _("Not found an archive (%s), containing file (%s).");

const wxString strUpdateAuthorCount = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id)");
const wxString strUpdateSequenCount = wxT("UPDATE sequences SET number=(SELECT COUNT(id_book) FROM bookseq WHERE bookseq.id_seq=sequences.id)");

const int ciNoAuthor = 0x7FFFFFFF;

const int ciMaxImageWidth = 200;

const wxString strRating [] = {
	wxT("Clear"),
	wxT("*"),
	wxT("* *"),
	wxT("* * *"),
	wxT("* * * *"),
	wxT("* * * * *"),
};
