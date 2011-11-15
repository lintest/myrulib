#ifndef __FBIMPORTCTX_H__
#define __FBIMPORTCTX_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/dynarray.h>
#include "FbDatabase.h"
#include "FbStringHash.h"

class AuthorItem
{
	public:
		AuthorItem(int id = 0): m_id(id) {};
		wxString GetFullName();
		void Convert(FbDatabase & database);
		int GetId() { return m_id; }
		int Find(FbDatabase & database);
		int Load(FbDatabase & database);
		int Save(FbDatabase & database);
		wxString first;
		wxString middle;
		wxString last;
	private:
		void SetNames();
		void Bind(wxSQLite3Statement &stmt, int param, const wxString &value);
	private:
		int m_id;
};

WX_DECLARE_OBJARRAY(AuthorItem, AuthorArray);

class SequenceItem
{
	public:
		SequenceItem(): m_id(0), m_number(0) {};
		SequenceItem(const FbStringHash &atts);
	public:
		int Convert(FbDatabase & database);
		int GetId() const { return m_id; }
		int GetNumber() const { return m_number; }
	private:
		int FindSequence(FbDatabase & database);
		int m_id;
		wxString m_name;
		long m_number;
};

WX_DECLARE_OBJARRAY(SequenceItem, SequenceArray);

#endif // FBIMPORTCTX_H__
