#ifndef __FBCOLLECTION_H__
#define __FBCOLLECTION_H__

#include <wx/wx.h>
#include "FbDatabase.h"

class FbModel;

class FbCasheData: public wxObject
{
	public:
		FbCasheData(wxSQLite3ResultSet &result);
		FbCasheData(int code, wxSQLite3ResultSet &result);
		int GetCode() const { return m_code; }
		wxString GetValue(FbModel & model, size_t col) const;
	private:
		int m_code;
		wxString m_name;
		int m_count;
		DECLARE_CLASS(FbCasheData)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbCasheData, FbCasheArray);

class FbCollection: public wxObject
{
	public:
		FbCollection();
		virtual ~FbCollection();
		FbCasheData * GetSeqn(int code);
		FbCasheData * GetAuth(int code);
		void AddSeqn(FbCasheData * data);
		void AddAuth(FbCasheData * data);
	protected:
	private:
		FbCommonDatabase m_database;
		FbCasheArray m_auths;
		FbCasheArray m_seqns;
		DECLARE_CLASS(FbCollection)
};

#endif // __FBCOLLECTION_H__
