#ifndef __FBCOLUMNS_H__
#define __FBCOLUMNS_H__

#include <wx/wx.h>
#include "FbBookTypes.h"

enum FbBookFields {
	BF_NAME = 0,
	BF_NUMB,
	BF_AUTH,
	BF_CODE,
	BF_GENR,
	BF_RATE,
	BF_LANG,
	BF_TYPE,
	BF_DATE,
	BF_SIZE,
	BF_BITE,
	BF_SEQN,
	BF_MD5S,
	BF_DOWN,
	BF_LAST,
};

class FbTreeViewCtrl;

class FbColumns
{
	public:
		static wxString GetName(int field);
		static int GetSize(int field);
		static wxAlignment GetAlign(int field);
		static wxChar GetCode(int field);
		static size_t GetCode(wxChar letter);
		static wxString Get(const wxArrayInt & columns);
		static void Set(const wxString &text, wxArrayInt & columns);
		static void Create(const wxArrayInt & columns, FbListMode mode, FbTreeViewCtrl & ctrl);
};

#endif // __FBCOLUMNS_H__
