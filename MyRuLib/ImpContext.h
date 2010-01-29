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
	AuthorItem(int id = 0): id(id) {};
	wxString GetFullName();
	void Convert(FbDatabase & database);
	int Find(FbDatabase & database);
	int Load(FbDatabase & database);
	int Save(FbDatabase & database);
private:
	void SetNames();
public:
	int id;
	wxString first;
	wxString middle;
	wxString last;
};

WX_DECLARE_OBJARRAY(AuthorItem, AuthorArray);

class SequenceItem
{
public:
	SequenceItem(): id(0), number(0) {};
	SequenceItem(const XML_Char **atts);
	int Convert(FbDatabase & database);
public:
	int id;
	wxString seqname;
	long number;
private:
	int FindSequence(FbDatabase & database);
};

WX_DECLARE_OBJARRAY(SequenceItem, SequenceArray);

#endif // __IMPCONTEXT_H__
