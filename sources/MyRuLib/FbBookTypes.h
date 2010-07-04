#ifndef __FBBOOKTYPES_H__
#define __FBBOOKTYPES_H__

enum FbFolderType {
	FT_FOLDER = 1,
	FT_RATING,
	FT_COMMENT,
	FT_DOWNLOAD,
	FT_AUTHOR,
	FT_GENRE,
	FT_SEQNAME,
	FT_DATE,
	FT_NOTHING,
};

enum FbListMode {
	FB2_MODE_LIST = 0,
	FB2_MODE_TREE = 1,
};

enum FbViewMode {
	FB2_VIEW_HORISONTAL = 0,
	FB2_VIEW_VERTICAL = 1,
	FB2_VIEW_NOTHING = 2,
};

enum FbMasterTypes {
	fbMT_NULL = 0,
	fbMT_BOOK,
	fbMT_AUTH,
	fbMT_SEQN,
	fbMT_GENR,
	fbMT_FLDR,
	fbMT_DOWN,
	fbMT_COMM,
	fbMT_RATE,
	fbMT_FIND,
	fbMT_DATE,
	fbMT_ARCH,
	fbMT_FILE,
	fbMT_LAST,
};

#include <wx/dynarray.h>
WX_DEFINE_SORTED_ARRAY_INT(int, FbSortedArrayInt);

#endif // __FBBOOKTYPE_H__
