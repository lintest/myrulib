#ifndef __BOOKINFO_H__
#define __BOOKINFO_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include "FbParser.h"

#define BIF_TITLE_INFO  0x0001
#define BIF_ANNOTATION  0x0002
#define BIF_DESCRIPTION 0x0003
#define BIF_COVER_IMAGE 0x0004

struct SeqItem {
public:
	SeqItem(int s, int n): seq(s), num(n) {};
	int seq;
	int num;

};

WX_DECLARE_OBJARRAY(SeqItem, SeqItemArray);

class BookInfo
{
private:
    bool m_ok;
	bool ReadXml(const FbDocument &xml, int flags);
	static int FindAuthor(wxString &full_name);
	static int FindSequence(wxString &name);
public:
	wxArrayInt authors;
	wxString title, annotation, genres;
	SeqItemArray sequences;
	static int NewId(int param);
    static void MakeLower(wxString & data);
    static void MakeUpper(wxString & data);
public:
    BookInfo(wxInputStream& stream, int flags = BIF_DESCRIPTION);
};

#endif // __BOOKINFO_H__
