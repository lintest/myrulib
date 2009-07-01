#ifndef __IMPCONTEXT_H__
#define __IMPCONTEXT_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>
#include "ParseCtx.h"

#define BIF_TITLE_INFO  0x0001
#define BIF_ANNOTATION  0x0002
#define BIF_DESCRIPTION 0x0003

class AuthorItem
{
private:
    static int FindAuthor(AuthorItem &author);
public:
    AuthorItem(): id(0) {};
    wxString GetFullName();
    void Convert(){ id = FindAuthor(*this); }
public:
    int id;
    wxString first;
    wxString middle;
    wxString last;
};

WX_DECLARE_OBJARRAY(AuthorItem, AuthorArray);

class SequenceItem
{
private:
    static int FindSequence(wxString &name);
public:
	SequenceItem(): id(0), number(0) {};
	SequenceItem(int s, int n): id(s), number(n) {};
    void Convert(){ id = FindSequence(seqname); }
public:
	int id;
	wxString seqname;
	long number;
};

WX_DECLARE_OBJARRAY(SequenceItem, SequenceArray);

class ImportParsingContext: public ParsingContext
{
public:
    wxString title;
    AuthorArray authors;
    SequenceArray sequences;
    wxString genres;
    AuthorItem * author;
    wxString text;
};

#endif // __IMPCONTEXT_H__
