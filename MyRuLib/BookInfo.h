#ifndef __BOOKINFO_H__
#define __BOOKINFO_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include "FbParser.h"

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
	bool ReadXml(const FbDocument &xml);
	static int FindAuthor(wxString &full_name);
	static int FindSequence(wxString &name);
    static void MakeLower(wxString & data);
    static void MakeUpper(wxString & data);
public:
	wxArrayInt authors;
	wxString title, annotation, genres;
	SeqItemArray sequences;
	static int NewId(int param);
public:
    BookInfo(const FbDocument &xml);
};

#endif // __BOOKINFO_H__
