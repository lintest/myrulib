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
	SequenceItem(const XML_Char **atts);
	void Convert(FbDatabase & database){ id = FindSequence(database); }
public:
	int id;
	wxString seqname;
	long number;
private:
	int FindSequence(FbDatabase & database);
};

WX_DECLARE_OBJARRAY(SequenceItem, SequenceArray);

#endif // __IMPCONTEXT_H__
