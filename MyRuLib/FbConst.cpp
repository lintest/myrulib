#include "FbConst.h"

const wxString strProgramName = wxT("MyRuLib 0.15");
const wxString strHomePage = wxT("http://myrulib.lintest.ru");
const wxString strMailAddr = wxT("mail@lintest.ru");

const wxString strVersionInfo = strHomePage +  wxT("\n") + strMailAddr;

const wxString alphabetRu = _("АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
const wxString alphabetEn = wxT("#ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const wxString strAlphabet = alphabetRu + alphabetEn;
const wxString strNobody = _("(без автора)");
const wxString strRusJE = wxT("е");
const wxString strRusJO = wxT("ё");
const wxString strOtherSequence = _("(разное)");
const wxString strBookNotFound = _("Не найден архив%s, содержащий файл%s.");

const wxString strUpdateCountSQL = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id)");


const int ciNoAuthor = 0x7FFFFFFF;

const int ciMaxImageWidth = 200;

const wxString strRating [] = {
	_("Очистить"),
	wxT("*"),
	wxT("* *"),
	wxT("* * *"),
	wxT("* * * *"),
	wxT("* * * * *"),
};
