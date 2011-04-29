#include <wx/wx.h>
#include <wx/wxsqlite3.h>

#define fbCF_AUTH 0x0001
#define fbCF_SEQN 0x0002
#define fbCF_DATE 0x0004
#define fbCF_ALL  0xFFFF


class FbCounter: public wxObject
{
	public:
		static void AddBook(wxSQLite3Database & database, int book);
		FbCounter(wxSQLite3Database & database, long style = fbCF_ALL);
		virtual ~FbCounter(void);
		void Add(const wxString & books);
		void Add(int book);
		void Execute();
	private:
		bool HasFlag(int flag) const
			{ return (m_style & flag) != 0; }
		void CreateTable(const wxString & name);
	private:
		wxSQLite3Database & m_database;
		long m_style;

};
