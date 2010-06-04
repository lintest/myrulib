#ifndef __FBDATABASE_H__
#define __FBDATABASE_H__

#include <wx/wxsqlite3.h>
#include "TestMain.h"

class FbSearchFunction: public wxSQLite3ScalarFunction
{
	public:
		FbSearchFunction(const wxString & input);
		static bool IsFullText(const wxString &text);
		static wxString AddAsterisk(const wxString &text);
	protected:
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		static void Decompose(const wxString &text, wxArrayString &list);
		wxArrayString m_masks;
};

class FbCommonDatabase: public wxSQLite3Database 
{
	public:
	   FbCommonDatabase()
		   { Open(DataViewFrame::sm_filename); }
};

#endif // __FBDATABASE_H__
