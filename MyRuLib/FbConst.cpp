#include "FbConst.h"

const wxString strProgramName = _T("MyRuLib 0.11");
const wxString strVersionInfo = _T("MyRuLib - version 0.11 (alpha)\n\nhttp://myrulib.lintest.ru\nmail@lintest.ru");
const wxString strHomePage = wxT("http://myrulib.lintest.ru");

const wxString alphabetRu = _("АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
const wxString alphabetEn = wxT("#ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const wxString strAlphabet = alphabetRu + alphabetEn;
const wxString strNobody = _("(без автора)");
const wxString strRusJE = wxT("е");
const wxString strRusJO = wxT("ё");
const wxString strOtherSequence = _("(разное)");
const wxString strBookNotFound = _("Не найден архив%s, содержащий файл%s.");

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
