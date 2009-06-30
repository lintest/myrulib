#ifndef __BOOKINFO_H__
#define __BOOKINFO_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>

#define BIF_TITLE_INFO  0x0001
#define BIF_ANNOTATION  0x0002
#define BIF_DESCRIPTION 0x0003

struct SeqItem {
public:
	SeqItem(int s, int n): seq(s), num(n) {};
	int seq;
	int num;

};

class AuthorItem
{
public:
    wxString GetFullName();
public:
    wxString first;
    wxString middle;
    wxString last;
};

WX_DECLARE_OBJARRAY(SeqItem, SeqItemArray);

class BookInfo
{
private:
    bool m_ok;
    bool Load(wxInputStream& stream);
	static int FindAuthor(wxString &full_name);
	static int FindAuthor(AuthorItem &author);
	static int FindSequence(wxString &name);
public:
	wxArrayInt authors;
	wxString title, genres;
	SeqItemArray sequences;
	static int NewId(int param);
    static void MakeLower(wxString & data);
    static void MakeUpper(wxString & data);
public:
    BookInfo(wxInputStream& stream, int flags = BIF_DESCRIPTION);
};

#endif // __BOOKINFO_H__
