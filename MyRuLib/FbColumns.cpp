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
		case BF_LAST: return wxEmptyString;
		default: return wxEmptyString;
	}
}
