#ifndef __IMPCONTEXT_H__
#define __IMPCONTEXT_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/thread.h>
#include "ParseCtx.h"
#include "FbDatabase.h"

class AuthorItem
{
public:
    AuthorItem(): id(0) {};
    wxString GetFullName();
    void Convert(FbDatabase & database) { id = FindAuthor(database); }
public:
    int id;
    wxString first;
    wxString middle;
    wxString last;
private:
    int FindAuthor(FbDatabase & database);
};

WX_DECLARE_OBJARRAY(AuthorItem, AuthorArray);

class SequenceItem
{
public:
	SequenceItem(): id(0), number(0) {};
	SequenceItem(int s, int n): id(s), number(n) {};
    void Convert(FbDatabase & database){ id = FindSequence(database); }
public:
	int id;
	wxString seqname;
	long number;
private:
    int FindSequence(FbDatabase & database);
};

WX_DECLARE_OBJARRAY(SequenceItem, SequenceArray);

class ImportParsingContext: public ParsingContext
{
public:
	ImportParsingContext(): md5only(false) {};
public:
    wxString title;
    AuthorArray authors;
    SequenceArray sequences;
    wxString genres;
    AuthorItem * author;
    wxString text;
    wxString md5sum;
    bool md5only;
    wxString filename;
    wxString filepath;
};

#endif // __IMPCONTEXT_H__
