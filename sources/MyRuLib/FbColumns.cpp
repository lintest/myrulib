#include "FbColumns.h"
#include "controls/FbTreeView.h"

wxString FbColumns::GetName(int field)
{
	switch (field) {
		case BF_NAME: return _("Title");
		case BF_AUTH: return _("Author");
		case BF_NUMB: return _("#");
		case BF_GENR: return _("Genre");
		case BF_LANG: return _("Language");
		case BF_TYPE: return _("Extension");
		case BF_SIZE: return _("Size, Kb");
		case BF_CODE: return _("Code");
		case BF_RATE: return _("Rating");
		case BF_DATE: return _("Date");
		case BF_BITE: return _("Size, byte");
		case BF_SEQN: return _("Ser.");
		case BF_MD5S: return _("MD5 Sum");
		case BF_DOWN: return _("Downloaded");
		case BF_LAST: return wxEmptyString;
		default: return wxEmptyString;
	}
}

int FbColumns::GetSize(int field)
{
	switch (field) {
		case BF_NAME: return 15;
		case BF_AUTH: return 10;
		case BF_NUMB: return  3;
		case BF_CODE: return  4;
		case BF_GENR: return  6;
		case BF_RATE: return  4;
		case BF_LANG: return  2;
		case BF_TYPE: return  3;
		case BF_DATE: return  6;
		case BF_SIZE: return  4;
		case BF_BITE: return  6;
		case BF_SEQN: return  6;
		case BF_MD5S: return 10;
		case BF_DOWN: return  6;
		case BF_LAST: return  4;
		default: return 4;
	}
}

wxAlignment FbColumns::GetAlign(int field)
{
	switch (field) {
		case BF_NAME: return wxALIGN_LEFT;
		case BF_NUMB: return wxALIGN_RIGHT;
		case BF_AUTH: return wxALIGN_LEFT;
		case BF_CODE: return wxALIGN_RIGHT;
		case BF_GENR: return wxALIGN_LEFT;
		case BF_RATE: return wxALIGN_LEFT;
		case BF_LANG: return wxALIGN_CENTER_HORIZONTAL;
		case BF_TYPE: return wxALIGN_CENTER_HORIZONTAL;
		case BF_DATE: return wxALIGN_CENTER_HORIZONTAL;
		case BF_SIZE: return wxALIGN_RIGHT;
		case BF_BITE: return wxALIGN_RIGHT;
		case BF_SEQN: return wxALIGN_LEFT;
		case BF_MD5S: return wxALIGN_LEFT;
		case BF_DOWN: return wxALIGN_CENTER_HORIZONTAL;
		case BF_LAST: return wxALIGN_LEFT;
		default: return wxALIGN_LEFT;
	}
}

int FbColumns::GetFixed(int field)
{
	switch (field) {
		case BF_NUMB: return  3;
		case BF_CODE: return  7;
		case BF_RATE: return  9;
		case BF_LANG: return  2;
		case BF_TYPE: return  4;
		case BF_DATE: return 10;
		case BF_SIZE: return 10;
		case BF_BITE: return 14;
		case BF_DOWN: return 10;
		default: return 0;
	}
}

wxChar FbColumns::GetChar(int field)
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

wxString FbColumns::Get(const wxArrayInt & columns)
{
	wxString result;
	size_t count = columns.Count();
	for (size_t i = 0; i < count; i++) {
		result << GetChar(columns[i]);
	}
	return result;
}

void FbColumns::Set(const wxString &text, wxArrayInt & columns)
{
	size_t length = text.Length();
	for (size_t i = 0; i < length; i++) {
		columns.Add(GetCode(text[i]));
	}
}

void FbColumns::Create(const wxArrayInt & columns, FbListMode mode, FbTreeViewCtrl & ctrl)
{
	ctrl.EmptyColumns();
	ctrl.AddColumn(BF_NAME, _("Title"), 15, wxALIGN_LEFT);
	size_t count = columns.Count();
	for (size_t i = 0; i < count; i++) {
		int index = columns[i];
		if (BF_AUTH <= index && index < BF_LAST) {
			ctrl.AddColumn(index, GetName(index), GetSize(index), GetAlign(index), GetFixed(index));
		}
	}
	ctrl.Refresh();
}
