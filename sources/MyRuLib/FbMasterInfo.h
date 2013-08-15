#ifndef __FBMASTERINFO_H__
#define __FBMASTERINFO_H__

#include <wx/wx.h>
#include "FbThread.h"
#include "FbColumns.h"
#include "FbBookTypes.h"
#include "FbFilterObj.h"
#include "FbDatabase.h"

class FbBookTreeModel;

class FbMasterInfoBase: public wxObject
{
	public:
		FbMasterInfoBase()
			: m_order(BF_NAME), m_mode(FB2_MODE_LIST), m_index(++sm_counter) {}
		FbMasterInfoBase(const FbMasterInfoBase & info)
			: m_order(info.m_order), m_mode(info.m_mode), m_index(info.m_index) {}
		virtual ~FbMasterInfoBase()
			{}
		int GetIndex() const
			{ return m_index; }
		void SetOrder(int order)
			{ m_order = order; }
		void SetMode(FbListMode mode)
			{ m_mode = mode; }
		virtual void * Execute(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter);
		virtual FbMasterInfoBase * Clone() const = 0;
		virtual bool operator==(const FbMasterInfoBase & info) = 0;
	protected:
		virtual wxString GetWhere(FbSQLite3Database &database) const = 0;
		virtual void Bind(FbSQLite3Statement &stmt) const {}
		virtual wxString GetOrderTable() const;
		virtual wxString GetOrderColumn() const;
		virtual wxString GetOrderFields() const;
		virtual wxString GetListSQL(FbSQLite3Database &database) const;
		virtual wxString GetTreeSQL(FbSQLite3Database &database) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, FbSQLite3ResultSet &result) const;
		virtual void MakeList(wxEvtHandler *owner, FbThread * thread, FbSQLite3ResultSet &result) const;
		void SendTree(wxEvtHandler *owner, FbThread * thread, FbBookTreeModel * model) const;
		wxString FormatSQL(const wxString &sql, const wxString &cond, const FbFilterObj &filter) const;
	protected:
		int GetOrder() const
			{ return m_order; }
		int GetOrderIndex() const
			{ return m_order ? abs(m_order) - 1 : 0; }
		FbListMode GetMode() const
			{ return m_mode; }
	private:
		static int sm_counter;
		int m_order;
		FbListMode m_mode;
		int m_index;
		DECLARE_CLASS(FbMasterInfoBase);
};

class FbMasterInfo: public wxObject
{
	public:
		FbMasterInfo()
			: m_data(NULL) {}
		FbMasterInfo(const FbMasterInfoBase & data)
			: m_data(data.Clone()) {}
		FbMasterInfo(const FbMasterInfo & info)
			: m_data(info ? info.m_data->Clone() : NULL) {}
		virtual ~FbMasterInfo()
			{ wxDELETE(m_data); }
		operator bool() const
			{ return m_data != NULL; }
		FbMasterInfoBase * operator &() const
			{ return m_data; }
		int GetIndex() const
			{ return m_data ? m_data->GetIndex() : 0; }
		void * Execute(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter)
			{ return m_data ? m_data->Execute(owner, thread, filter) : NULL; }
		void SetOrder(int order)
			{ if (m_data) m_data->SetOrder(order); }
		void SetMode(FbListMode mode)
			{ if (m_data) m_data->SetMode(mode); }
		FbMasterInfo & operator =(const FbMasterInfo &info);
		bool operator ==(const FbMasterInfo &info) const;
	private:
		FbMasterInfoBase * m_data;
		DECLARE_CLASS(FbMasterInfo);
};

#endif // __FBMASTERINFO_H__
