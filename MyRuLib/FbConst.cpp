#include "FbConst.h"

#include "version.inc"

#define __TXT(text) #text
#define TXT(text) __TXT(text)

const wxString strProgramName = wxT("MyRuLib " TXT(VERSION_1) "." TXT(VERSION_2));
const wxString strProgramInfo = strProgramName + wxT("." TXT(VERSION_3) "." TXT(VERSION_4)) + wxT(" (alpha)");
const wxString strHomePage = wxT("http://myrulib.lintest.ru");
const wxString strMailAddr = wxT("mail@lintest.ru");

const wxString strVersionInfo = strHomePage +  wxT("\n") + strMailAddr;

#if wxCHECK_VERSION(2, 9, 0)
const wxString alphabetRu = _(L"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
#else
const wxString alphabetRu = _("АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
#endif

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
