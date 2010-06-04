#ifndef __FBCOLLECTION_H__
#define __FBCOLLECTION_H__

#include <wx/wx.h>

class FbCasheData: public wxObjects
{
	public:
		FbCasheData(wxSQLite3ResultSet &result);
		FbCasheData(int code, wxSQLite3ResultSet &result);
		int GetCode() const { return m_code }
	private:
		int m_code;
		wxString m_name;
		int m_count;
		DECLARE_CLASS(FbCasheData)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbCasheData, FbCasheArray);

class FbCollection: public wxObjects
{
	public:
		FbCollection();
		virtual ~FbCollection();
		FbCasheData * GetSeqn(int code);
		FbCasheData * GetAuth(int code);
	protected:
	private:
		FbCasheArray m_auths;
		FbCasheArray m_seqns;
		DECLARE_CLASS(FbCollection)
};

#endif // __FBCOLLECTION_H__
