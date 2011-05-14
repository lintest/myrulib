#ifndef __FBBOOKTREE_H__
#define __FBBOOKTREE_H__

#include "controls/FbTreeModel.h"
#include "FbBookTypes.h"
#include "FbCollection.h"
#include "FbThread.h"
#include "FbMasterInfo.h"
#include "FbBookTraverser.h"

class FbAuthParentData: public FbParentData
{
	public:
		FbAuthParentData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code), m_state(0) {}
		int GetCode() const
			{ return m_code; }
		virtual bool FullRow(FbModel & model) const
			{ return true; }
		virtual bool HiddenRoot() const
			{ return false; }
		virtual FbViewItem GetView() const
			{ return FbViewItem(FbViewItem::Auth, m_code); }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return FbCollection::GetAuth(m_code, 0); }
	public:
		int Compare(const FbAuthParentData &data) const;
		void SortItems();
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
		DECLARE_CLASS(FbAuthParentData);
};

class FbSeqnParentData: public FbParentData
{
	public:
		FbSeqnParentData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code), m_state(0) {}
		int GetCode() const
			{ return m_code; }
		virtual bool FullRow(FbModel & model) const
			{ return true; }
		virtual bool HiddenRoot() const
			{ return false; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return FbCollection::GetSeqn(m_code, 0); }
	public:
		int Compare(const FbSeqnParentData &data) const;
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
			: FbChildData(model, parent), m_code(code), m_numb(numb), m_state(0) {}
		int GetCode() const
			{ return m_code; }
		virtual int GetBook() const
			{ return m_code; }
		virtual FbViewItem GetView() const
			{ return FbViewItem(FbViewItem::Book, m_code); }
		virtual bool IsGray(FbModel & model) const
			{ return FbCollection::GetBookData(m_code).IsGray(); }
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

class FbBookTreeModel: public FbTreeModel
{
	public:
		FbBookTreeModel() {}
		virtual size_t GetSelected(wxArrayInt &items);
		virtual int GetBook();
		virtual FbViewItem GetView();
		virtual void Delete();
		int GetBookCount();
		wxString GetText(wxArrayInt &columns);
	private:
		size_t DoTraverse(FbBookTraverser & traverser);
		size_t GetChecked(FbBookTraverser & traverser, FbModelItem &parent, int level, size_t &row);
		size_t GetSelected(FbBookTraverser & traverser, FbModelItem &parent, int level, size_t &row);
		void GetBooks(FbModelItem &parent, FbSortedArrayInt &items);
		bool DoDelete(FbModelItem &parent, size_t &row);
		DECLARE_CLASS(FbBookTreeModel);
};

#endif // __FBBOOKTREE_H__
