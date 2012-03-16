#ifndef __FBGENRTREE_H__
#define __FBGENRTREE_H__

#include "controls/FbTreeModel.h"
#include "FbFrameThread.h"

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
		FbGenrParentData(FbModel & model, FbParentData * parent, const wxString &code, const wxString &name)
			: FbParentData(model, parent), m_code(code), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col) const
			{ return col ? (wxString)wxEmptyString : m_name; }
		wxString GetCode() const
			{ return m_code; }
	private:
		wxString m_code;
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
		FbMasterInfo GetInfo(FbModel & model) const;
	private:
		wxString m_code;
		wxString m_name;
		int m_count;
		DECLARE_CLASS(FbGenrChildData);
};

class FbGenrListThread: public FbFrameThread
{
	public:
		FbGenrListThread(wxEvtHandler * frame)
			: FbFrameThread(frame, wxEmptyString) {}
	protected:
		virtual void * Entry();
		virtual void MakeModel(wxSQLite3ResultSet &result) {}
};

#endif // __FBGENRTREE_H__

