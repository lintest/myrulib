#ifndef __FBMASTERINFO_H__
#define __FBMASTERINFO_H__

#include <wx/object.h>
#include "FbThread.h"
#include "FbBookTypes.h"
#include "FbCollection.h"

class FbMasterInfo : public wxObject
{
	public:
		FbMasterInfo(int index)
			: m_order(0), m_mode(FB2_MODE_LIST), m_index(index) {}
		FbMasterInfo(const FbMasterInfo & info)
			: m_order(info.m_order), m_mode(info.m_mode), m_index(info.m_index) {}
		virtual ~FbMasterInfo()
			{}
		int GetIndex() const
			{ return m_index; }
		FbBookFields GetOrder() const
			{ return m_order; }
		FbMasterInfo & SetOrder(FbBookFields order)
			{ m_order = order; return *this; }
		FbListMode GetMode() const
			{ return m_mode; }
		FbMasterInfo & SetMode(FbListMode mode)
			{ m_mode = mode; return *this; }
		virtual void * Execute(wxEvtHandler * owner, FbThread * thread) const;
	public:
		virtual FbMasterInfo * Clone() const = 0;
		virtual wxString GetSQL(wxSQLite3Database &database) const = 0;
		virtual void Bind(wxSQLite3Statement &stmt) const {}
		virtual void MakeTree(wxEvtHandler *owner, wxSQLite3ResultSet &result) const;
		virtual void MakeList(wxEvtHandler *owner, wxSQLite3ResultSet &result) const;
	private:
		FbBookFields m_order;
		FbListMode m_mode;
		int m_index;
		DECLARE_CLASS(FbMasterInfo);
};

class FbMasterAuthorInfo: public FbMasterInfo
{
	public:
		FbMasterAuthorInfo(int index, int id)
			: FbMasterInfo(index), m_id(id) {}
		FbMasterAuthorInfo(const FbMasterAuthorInfo &info)
			: FbMasterInfo(info), m_id(info.m_id) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterAuthorInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterAuthorInfo);
};

class FbMasterSeqnInfo: public FbMasterInfo
{
	public:
		FbMasterSeqnInfo(int index, int id)
			: FbMasterInfo(index), m_id(id) {}
		FbMasterSeqnInfo(const FbMasterSeqnInfo &info)
			: FbMasterInfo(info), m_id(info.m_id) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterSeqnInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterSeqnInfo);
};

class FbMasterGenrInfo: public FbMasterInfo
{
	public:
		FbMasterGenrInfo(int index, const wxString &id)
			: FbMasterInfo(index), m_id(id) {}
		FbMasterGenrInfo(const FbMasterGenrInfo &info)
			: FbMasterInfo(info), m_id(info.m_id) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterGenrInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		const wxString m_id;
		DECLARE_CLASS(FbMasterGenrInfo);
};

class FbMasterDownInfo: public FbMasterInfo
{
	public:
		FbMasterDownInfo(int index, int id)
			: FbMasterInfo(index), m_id(id) {}
		FbMasterDownInfo(const FbMasterDownInfo &info)
			: FbMasterInfo(info), m_id(info.m_id) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterDownInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterDownInfo);
};

class FbMasterSearchInfo: public FbMasterInfo
{
	public:
		FbMasterSearchInfo(int index, const wxString &title, const wxString &author = wxEmptyString)
			: FbMasterInfo(index), m_title(title), m_author(author) {}
		FbMasterSearchInfo(const FbMasterSearchInfo &info)
			: FbMasterInfo(info), m_title(info.m_title), m_author(info.m_author) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterSearchInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		wxString m_title;
		wxString m_author;
		DECLARE_CLASS(FbMasterSearchInfo);
};

class FbMasterDateInfo: public FbMasterInfo
{
	public:
		FbMasterDateInfo(int index, int id)
			: FbMasterInfo(index), m_id(id) {}
		FbMasterDateInfo(const FbMasterDateInfo &info)
			: FbMasterInfo(info), m_id(info.m_id) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterDateInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterDateInfo);
};

class FbMasterFldrInfo: public FbMasterInfo
{
	public:
		FbMasterFldrInfo(int index, int id, FbFolderType type)
			: FbMasterInfo(index), m_id(id), m_type(type) {}
		FbMasterFldrInfo(const FbMasterFldrInfo &info)
			: FbMasterInfo(info), m_id(info.m_id), m_type(info.m_type) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterFldrInfo(*this); }
		virtual wxString GetSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		FbFolderType m_type;
		DECLARE_CLASS(FbMasterFldrInfo);
};

#endif // __FBMASTERINFO_H__
