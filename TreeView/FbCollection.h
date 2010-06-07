#ifndef __FBCOLLECTION_H__
#define __FBCOLLECTION_H__

#include <wx/wx.h>
#include "FbDatabase.h"

class FbModel;

class FbCacheData: public wxObject
{
	public:
		FbCacheData(wxSQLite3ResultSet &result);
		FbCacheData(int code, wxSQLite3ResultSet &result);
		int GetCode() const { return m_code; }
		wxString GetValue(size_t col) const;
	private:
		int m_code;
		wxString m_name;
		int m_count;
		DECLARE_CLASS(FbCacheData)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbCacheData, FbCasheArray);

class FbCollection: public wxObject
{
	public:
		static wxString Format(int number);
		FbCollection(const wxString &filename);
		virtual ~FbCollection();
	public:
		static FbCollection * GetCollection();
		static FbCacheData * GetSeqn(int code);
		static FbCacheData * GetAuth(int code);
		static void AddSeqn(FbCacheData * data);
		static void AddAuth(FbCacheData * data);
	protected:
		FbCacheData * GetData(int code, FbCasheArray &items, const wxString &sql);
		void AddData(FbCasheArray &items, FbCacheData * data);
	private:
		static wxCriticalSection sm_section;
		FbCommonDatabase m_database;
		FbCasheArray m_auths;
		FbCasheArray m_seqns;
		DECLARE_CLASS(FbCollection)
};

#endif // __FBCOLLECTION_H__
