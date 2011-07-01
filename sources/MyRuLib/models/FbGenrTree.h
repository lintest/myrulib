#ifndef __FBGENRTREE_H__
#define __FBGENRTREE_H__

#include "controls/FbTreeModel.h"
#include "FbThread.h"

class wxSQLite3ResultSet;

class FbGenrListData: public FbModelData
{
	public:
		FbGenrListData(wxSQLite3ResultSet & result);
	private:
		wxString m_code;
		int m_count;
		friend class FbGenrChildData;
		DECLARE_CLASS(FbGenrListData);
};

class FbGenrParentData: public FbParentData
{
	public:
		FbGenrParentData(FbModel & model, FbParentData * parent, const wxString &name)
			: FbParentData(model, parent), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col) const
			{ return col ? (wxString)wxEmptyString : m_name; }
	private:
		wxString m_name;
		DECLARE_CLASS(FbGenrParentData);
};

class FbGenrChildData: public FbChildData
{
	public:
		FbGenrChildData(FbModel & model, FbParentData * parent, const wxString &code, const wxString &name)
			: FbChildData(model, parent), m_code(code), m_name(name), m_count(0) {}
		wxString GetCode() const
			{ return m_code; }
		void SetCount(int count)
			{ m_count = count; }
		bool operator==(const FbGenrListData & data) const
			{ return m_code == data.m_code; }
		FbGenrChildData & operator=(const FbGenrListData & data)
			{ if (!m_count) m_count = data.m_count; return *this; }
		bool operator==(const FbMasterInfo & info) const;
		virtual wxString GetValue(FbModel & model, size_t col) const;
		FbMasterInfo GetInfo() const;
	private:
		wxString m_code;
		wxString m_name;
		int m_count;
		DECLARE_CLASS(FbGenrChildData);
};

class FbGenrListThread: public FbThread
{
	public:
		FbGenrListThread(wxEvtHandler * frame)
			: FbThread(wxTHREAD_JOINABLE), m_frame(frame) {}
	protected:
		virtual void * Entry();
	private:
		wxEvtHandler * m_frame;
};

#endif // __FBGENRTREE_H__

