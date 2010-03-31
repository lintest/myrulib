#include "FbConst.h"

const wxString strProgramName = wxT("MyRuLib 0.21");
const wxString strHomePage = wxT("http://myrulib.lintest.ru");
const wxString strMailAddr = wxT("mail@lintest.ru");

const wxString strVersionInfo = strHomePage +  wxT("\n") + strMailAddr;

#if wxCHECK_VERSION(2, 9, 0)
const wxString alphabetRu = _(L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
#else
const wxString alphabetRu = _("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
#endif

const wxString alphabetEn = wxT("#ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const wxString strAlphabet = alphabetRu + alphabetEn;
const wxString strNobody = _("(no Author)");
const wxString strRusJE = wxT("е");
const wxString strRusJO = wxT("ё");
const wxString strOtherSequence = _("(Misc.)");
const wxString strBookNotFound = _("Not found an archive (%s), containing file (%s).");

const wxString strUpdateAuthorCount = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id)");
const wxString strUpdateSequenCount = wxT("UPDATE sequences SET number=(SELECT COUNT(id_book) FROM bookseq WHERE bookseq.id_seq=sequences.id)");

const int ciNoAuthor = 0x7FFFFFFF;

const int ciMaxImageWidth = 200;

const wxString strRating [] = {
	_("Clear"),
	wxT("*"),
	wxT("* *"),
	wxT("* * *"),
	wxT("* * * *"),
	wxT("* * * * *"),
};
