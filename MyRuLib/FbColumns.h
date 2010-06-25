#ifndef __FBCOLUMNS_H__
#define __FBCOLUMNS_H__

#include <wx/wx.h>

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

class FbColumns
{
	public:
		static wxString GetName(size_t field);
		static wxString GetSize(size_t field);
};

#endif // __FBCOLUMNS_H__
