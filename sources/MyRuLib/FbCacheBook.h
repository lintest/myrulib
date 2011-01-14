#ifndef __FBCACHEBOOK_H__
#define __FBCACHEBOOK_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

class FbCacheBook: public wxObject
{
	public:
		static FbCacheBook Get(int code, wxSQLite3Database & database);
		FbCacheBook(int code = 0);
		FbCacheBook(int code, wxSQLite3ResultSet &result);
		FbCacheBook(const FbCacheBook &book);
		FbCacheBook & operator =(const FbCacheBook &book);
		operator bool() const { return m_code; }
		int GetCode() const { return m_code; }
		wxString GetValue(size_t field) const;
		bool IsGray() const;
	private:
		static wxString GetSQL();
		int m_code;
		wxString m_name;
		wxString m_auth;
		wxString m_genr;
		wxString m_lang;
		wxString m_type;
		wxString m_md5s;
		int m_rate;
		int m_date;
		int m_size;
		int m_down;
		bool m_gray;
		DECLARE_CLASS(FbCacheBook)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbCacheBook, FbCasheBookArray);

#endif // __FBCACHEBOOK_H__
