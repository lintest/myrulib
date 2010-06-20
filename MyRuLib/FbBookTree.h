#ifndef __FBBOOKTREE_H__
#define __FBBOOKTREE_H__

#include "FbTreeModel.h"
#include "FbBookTypes.h"
#include "FbCollection.h"
#include "FbThread.h"

class FbBookTreeThread: public FbThread
{
	public:
		FbBookTreeThread(wxEvtHandler * frame, int author)
			:FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_author(author) {}
	protected:
		virtual void * Entry();
		void MakeModel(wxSQLite3Database &database, wxSQLite3ResultSet &result);
		void DoAuthor(wxSQLite3Database &database);
	private:
		wxEvtHandler * m_frame;
		int m_author;
};

class FbAuthParentData: public FbParentData
{
	public:
		FbAuthParentData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code) {}
		int GetCode() const
			{ return m_code; }
		virtual bool FullRow(FbModel & model) const
			{ return true; }
		virtual bool HiddenRoot() const
			{ return false; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return FbCollection::GetAuth(m_code, 0); }
		void SortItems(wxSQLite3Database &database);
	protected:
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
	private:
		int m_code;
		int m_state;
		DECLARE_CLASS(FbAuthParentData);
};

class FbSeqnParentData: public FbParentData
{
	public:
		FbSeqnParentData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code) {}
		int GetCode() const
			{ return m_code; }
		virtual bool FullRow(FbModel & model) const
			{ return true; }
		virtual bool HiddenRoot() const
			{ return false; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return FbCollection::GetSeqn(m_code, 0); }
		int Compare(const FbSeqnParentData &data);
	protected:
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
		wxString GetTitle() const
			{ return FbCollection::GetSeqn(m_code, 0); }
	private:
		int m_code;
		int m_state;
		DECLARE_CLASS(FbSeqnParentData);
};

class FbBookChildData: public FbChildData
{
	public:
		FbBookChildData(FbModel & model, FbParentData * parent, int code, int numb)
			: FbChildData(model, parent), m_code(code), m_numb(numb) {}
		int GetCode() const
			{ return m_code; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
	protected:
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
	private:
		int m_code;
		int m_numb;
		int m_state;
		DECLARE_CLASS(FbBookChildData);
};

#endif // __FBBOOKTREE_H__
