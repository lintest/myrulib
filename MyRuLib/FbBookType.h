#ifndef __FBBOOKTYPE_H__
#define __FBBOOKTYPE_H__

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

#endif // __FBBOOKTYPE_H__
