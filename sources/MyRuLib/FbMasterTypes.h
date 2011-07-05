#ifndef __FBMASTERTYPES_H__
#define __FBMASTERTYPES_H__

#include "FbMasterInfo.h"

class FbMasterAuthInfo: public FbMasterInfoBase
{
	public:
		FbMasterAuthInfo(int id)
			: m_id(id) {}
		FbMasterAuthInfo(const FbMasterAuthInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterAuthInfo(*this); }
		int GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterAuthInfo * data = wxDynamicCast(&info, FbMasterAuthInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual wxString GetOrderTable() const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterAuthInfo);
};

class FbMasterSeqnInfo: public FbMasterInfoBase
{
	public:
		FbMasterSeqnInfo(int id)
			: m_id(id) {}
		FbMasterSeqnInfo(const FbMasterSeqnInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterSeqnInfo(*this); }
		int GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterSeqnInfo * data = wxDynamicCast(&info, FbMasterSeqnInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterSeqnInfo);
};

class FbMasterGenrInfo: public FbMasterInfoBase
{
	public:
		FbMasterGenrInfo(const wxString &id)
			: m_id(id) {}
		FbMasterGenrInfo(const FbMasterGenrInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterGenrInfo(*this); }
		wxString GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterGenrInfo * data = wxDynamicCast(&info, FbMasterGenrInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		const wxString m_id;
		DECLARE_CLASS(FbMasterGenrInfo);
};

class FbMasterDownInfo: public FbMasterInfoBase
{
	public:
		enum DownType {
			DT_WAIT,
			DT_READY,
			DT_ERROR,
		};
		FbMasterDownInfo(DownType id)
			: m_id(id) {}
		FbMasterDownInfo(const FbMasterDownInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterDownInfo(*this); }
		DownType GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterDownInfo * data = wxDynamicCast(&info, FbMasterDownInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		DownType m_id;
		DECLARE_CLASS(FbMasterDownInfo);
};

class FbMasterDateInfo: public FbMasterInfoBase
{
	public:
		FbMasterDateInfo(int id)
			: m_id(id) {}
		FbMasterDateInfo(const FbMasterDateInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterDateInfo(*this); }
		int GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterDateInfo * data = wxDynamicCast(&info, FbMasterDateInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterDateInfo);
};

class FbMasterFldrInfo: public FbMasterInfoBase
{
	public:
		FbMasterFldrInfo(int id)
			: m_id(id) {}
		FbMasterFldrInfo(const FbMasterFldrInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterFldrInfo(*this); }
		int GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterFldrInfo * data = wxDynamicCast(&info, FbMasterFldrInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterFldrInfo);
};

class FbMasterCommInfo: public FbMasterInfoBase
{
	public:
		FbMasterCommInfo()
			{}
		FbMasterCommInfo(const FbMasterCommInfo &info)
			: FbMasterInfoBase(info) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterCommInfo(*this); }
		virtual bool operator==(const FbMasterInfoBase & info) {
			return IsSameAs(info);
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		DECLARE_CLASS(FbMasterCommInfo);
};

class FbMasterRateInfo: public FbMasterInfoBase
{
	public:
		FbMasterRateInfo(int id)
			: m_id(id) {}
		FbMasterRateInfo(const FbMasterRateInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterRateInfo(*this); }
		int GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterRateInfo * data = wxDynamicCast(&info, FbMasterRateInfo);
			return data && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterRateInfo);
};

class FbMasterFindInfo: public FbMasterInfoBase
{
	public:
		FbMasterFindInfo(const wxString &title, const wxString &author = wxEmptyString)
			: m_title(title), m_author(author) {}
		FbMasterFindInfo(const FbMasterFindInfo &info)
			: FbMasterInfoBase(info), m_title(info.m_title), m_author(info.m_author) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterFindInfo(*this); }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterFindInfo * data = wxDynamicCast(&info, FbMasterFindInfo);
			return data && data->m_title == m_title && data->m_author == m_author;
		}
	protected:
		virtual void * Execute(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter);
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		bool DoFind(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter);
	private:
		wxString m_title;
		wxString m_author;
		bool m_full;
		bool m_auth;
		DECLARE_CLASS(FbMasterFindInfo);
};

class FbMasterClssInfo: public FbMasterInfoBase
{
	public:
		FbMasterClssInfo(const wxString &sql, const wxString &id)
			: m_sql(sql), m_id(id) {}
		FbMasterClssInfo(const FbMasterClssInfo & info)
			: FbMasterInfoBase(info), m_sql(info.m_sql), m_id(info.m_id) {}
		virtual FbMasterClssInfo * Clone() const
			{ return new FbMasterClssInfo(*this); }
		wxString GetId() const
			{ return m_id; }
		virtual bool operator==(const FbMasterInfoBase & info) {
			const FbMasterClssInfo * data = wxDynamicCast(&info, FbMasterClssInfo);
			return data && data->m_sql == m_sql && data->m_id == m_id;
		}
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		const wxString m_sql;
		const wxString m_id;
		DECLARE_CLASS(FbMasterClssInfo);
};

#endif // __FBMASTERTYPES_H__
