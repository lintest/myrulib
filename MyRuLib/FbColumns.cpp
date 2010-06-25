#include "FbColumns.h"

wxString FbColumns::GetName(size_t field)
{
	switch (field) {
		case BF_NAME: return _("Title");
		case BF_NUMB: return _("#");
		case BF_AUTH: return _("Author");
		case BF_CODE: return _("Code");
		case BF_GENR: return _("Genre");
		case BF_RATE: return _("Rating");
		case BF_LANG: return _("Language");
		case BF_TYPE: return _("Extension");
		case BF_DATE: return _("Date");
		case BF_SIZE: return _("Size, Kb");
		case BF_BITE: return _("Size, byte");
		case BF_SEQN: return _("Sequence");
		case BF_MD5S: return _("MD5 Sum");
		case BF_DOWN: return _("Downloaded");
		case BF_LAST: return wxEmptyString;
		default: return wxEmptyString;
	}
}

wxChar FbColumns::GetCode(size_t field)
{
	if (BF_AUTH <= field && field < BF_LAST)
		return wxT('A') + (field - BF_AUTH);
	else return 0;
}

size_t FbColumns::GetCode(wxChar letter)
{
	int delta = letter - wxT('A');
	if (0 <= delta && delta < (BF_LAST - BF_AUTH))
		return BF_AUTH + delta;
	else return 0;
}

