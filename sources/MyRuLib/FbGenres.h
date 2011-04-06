#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "controls/FbTreeModel.h"
#include "FbStringHash.h"

class FbGenreGroup: public wxObject
{
	public:
		FbGenreGroup(const wxString &name)
			: m_name(name) {}
		void Add(const wxString &code)
			{ m_items.Add(code); }
	private:
		wxString m_name;
		wxArrayString m_items;
		friend class FbGenres;
		DECLARE_CLASS(FbGenreGroup);
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY( FbGenreGroup, FbGenreArray );

class FbGenres: public wxObject
{
	public:
		static void Init();
		static wxString Char(const wxString &code);
		static wxString Name(const wxString &letter);
		static wxString DecodeList(const wxString &genres);
		static void GetNames(FbStringHash & names);
		static FbModel * CreateModel();
	private:
		static wxCriticalSection sm_section;
		static FbGenreArray sm_groups;
		static FbStringHash sm_chars;
		static FbStringHash sm_names;
		DECLARE_CLASS(FbGenres);
};

class FbGenreFunction : public wxSQLite3ScalarFunction
{
	public:
		FbGenreFunction() { FbGenres::GetNames(m_names); }
	protected:
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		wxString DecodeList(const wxString &genres);
		FbStringHash m_names;
};

#endif //FBGENRES_H

