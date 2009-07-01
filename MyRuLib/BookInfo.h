#ifndef __BOOKINFO_H__
#define __BOOKINFO_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>

#define BIF_TITLE_INFO  0x0001
#define BIF_ANNOTATION  0x0002
#define BIF_DESCRIPTION 0x0003

struct SeqItem
{
public:
	SeqItem(): id(0), number(0) {};
	SeqItem(int s, int n): id(s), number(n) {};
public:
	int id;
	wxString seqname;
	long number;
};

class AuthorItem
{
public:
    AuthorItem(): id(0) {};
    wxString GetFullName();
public:
    int id;
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
