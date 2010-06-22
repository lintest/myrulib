#ifndef __FBMASTERINFO_H__
#define __FBMASTERINFO_H__

#include <wx/object.h>
#include "FbThread.h"
#include "FbBookTypes.h"
#include "FbCollection.h"

class FbMasterInfoPtr: public wxObject
{
	public:
		FbMasterInfoPtr(int index)
			: m_order(BF_NAME), m_mode(FB2_MODE_LIST), m_index(index) {}
		FbMasterInfoPtr(const FbMasterInfoPtr & info)
			: m_order(info.m_order), m_mode(info.m_mode), m_index(info.m_index) {}
		virtual ~FbMasterInfoPtr()
			{}
		int GetIndex() const
			{ return m_index; }
		void SetOrder(int order)
			{ m_order = order; }
		void SetMode(FbListMode mode)
			{ m_mode = mode; }
		virtual void * Execute(wxEvtHandler * owner, FbThread * thread);
		virtual FbMasterInfoPtr * Clone() const = 0;
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const = 0;
		virtual void Bind(wxSQLite3Statement &stmt) const {}
		virtual wxString GetSelectColumn() const;
		virtual wxString GetOrderTable() const;
		virtual wxString GetOrderColumn() const;
		virtual wxString GetOrderFields() const;
		virtual wxString GetListSQL(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
		virtual void MakeList(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
		wxString FormatListSQL(const wxString &sql, const wxString &cond) const;
		wxString FormatTreeSQL(const wxString &sql, const wxString &cond) const;
	protected:
		int GetOrder() const
			{ return m_order; }
		int GetOrderIndex() const
			{ return m_order ? abs(m_order) - 1 : 0; }
		FbListMode GetMode() const
			{ return m_mode; }
	private:
		int m_order;
		FbListMode m_mode;
		int m_index;
		DECLARE_CLASS(FbMasterInfoPtr);
};

class FbMasterInfo: public wxObject
{
	public:
		FbMasterInfo()
			: m_data(NULL) {}
		FbMasterInfo(FbMasterInfoPtr * data)
			: m_data(data) {}
		FbMasterInfo(const FbMasterInfo &info)
			: m_data(info ? info.m_data->Clone() : NULL) {}
		FbMasterInfo & operator =(const FbMasterInfo &info)
			{ wxDELETE(m_data); m_data = info ? info.m_data->Clone() : NULL; return *this; }
		virtual ~FbMasterInfo()
			{ wxDELETE(m_data); }
		operator bool() const
			{ return m_data != NULL; }
		int GetIndex() const
			{ return m_data ? m_data->GetIndex() : 0; }
		void * Execute(wxEvtHandler * owner, FbThread * thread)
			{ return m_data ? m_data->Execute(owner, thread) : NULL; }
		void SetOrder(int order)
			{ if (m_data) m_data->SetOrder(order); }
		void SetMode(FbListMode mode)
			{ if (m_data) m_data->SetMode(mode); }
	private:
		FbMasterInfoPtr * m_data;
		DECLARE_CLASS(FbMasterInfoPtr);
};

class FbMasterAuthorInfo: public FbMasterInfoPtr
{
	public:
		FbMasterAuthorInfo(int index, int id)
			: FbMasterInfoPtr(index), m_id(id) {}
		FbMasterAuthorInfo(const FbMasterAuthorInfo &info)
			: FbMasterInfoPtr(info), m_id(info.m_id) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterAuthorInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual wxString GetOrderTable() const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterAuthorInfo);
};

class FbMasterSeqnInfo: public FbMasterInfoPtr
{
	public:
		FbMasterSeqnInfo(int index, int id)
			: FbMasterInfoPtr(index), m_id(id) {}
		FbMasterSeqnInfo(const FbMasterSeqnInfo &info)
			: FbMasterInfoPtr(info), m_id(info.m_id) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterSeqnInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterSeqnInfo);
};

class FbMasterGenrInfo: public FbMasterInfoPtr
{
	public:
		FbMasterGenrInfo(int index, const wxString &id)
			: FbMasterInfoPtr(index), m_id(id) {}
		FbMasterGenrInfo(const FbMasterGenrInfo &info)
			: FbMasterInfoPtr(info), m_id(info.m_id) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterGenrInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		const wxString m_id;
		DECLARE_CLASS(FbMasterGenrInfo);
};

class FbMasterDownInfo: public FbMasterInfoPtr
{
	public:
		FbMasterDownInfo(int index, int id)
			: FbMasterInfoPtr(index), m_id(id) {}
		FbMasterDownInfo(const FbMasterDownInfo &info)
			: FbMasterInfoPtr(info), m_id(info.m_id) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterDownInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterDownInfo);
};

class FbMasterDateInfo: public FbMasterInfoPtr
{
	public:
		FbMasterDateInfo(int index, int id)
			: FbMasterInfoPtr(index), m_id(id) {}
		FbMasterDateInfo(const FbMasterDateInfo &info)
			: FbMasterInfoPtr(info), m_id(info.m_id) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterDateInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterDateInfo);
};

class FbMasterFldrInfo: public FbMasterInfoPtr
{
	public:
		FbMasterFldrInfo(int index, int id, FbFolderType type)
			: FbMasterInfoPtr(index), m_id(id), m_type(type) {}
		FbMasterFldrInfo(const FbMasterFldrInfo &info)
			: FbMasterInfoPtr(info), m_id(info.m_id), m_type(info.m_type) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterFldrInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		FbFolderType m_type;
		DECLARE_CLASS(FbMasterFldrInfo);
};

class FbMasterSearchInfo: public FbMasterInfoPtr
{
	public:
		FbMasterSearchInfo(int index, const wxString &title, const wxString &author = wxEmptyString)
			: FbMasterInfoPtr(index), m_title(title), m_author(author) {}
		FbMasterSearchInfo(const FbMasterSearchInfo &info)
			: FbMasterInfoPtr(info), m_title(info.m_title), m_author(info.m_author) {}
		virtual FbMasterInfoPtr * Clone() const
			{ return new FbMasterSearchInfo(*this); }
	protected:
		virtual void * Execute(wxEvtHandler * owner, FbThread * thread);
		virtual wxString GetOrderTable() const;
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		wxString m_title;
		wxString m_author;
		bool m_full;
		bool m_auth;
		DECLARE_CLASS(FbMasterSearchInfo);
};

#endif // __FBMASTERINFO_H__
