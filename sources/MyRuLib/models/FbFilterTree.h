#ifndef __FBFILTERTREE_H__
#define __FBFILTERTREE_H__

#include "controls/FbTreeModel.h"
#include "FbThread.h"

class FbFilterParentData: public FbParentData
{
	public:
		FbFilterParentData(FbModel & model)
			: FbParentData(model, NULL), m_name(_("All")) {}
		virtual bool HiddenRoot() const
			{ return false; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return m_name; }
	protected:
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
	private:
		wxString m_name;
		int m_state;
		DECLARE_CLASS(FbFilterParentData)
};

class FbFilterChildData: public FbChildData
{
	public:
		FbFilterChildData(FbModel & model, FbParentData * parent, const wxString &code)
			: FbChildData(model, parent), m_code(code), m_state(0) {}
		wxString GetCode() const
			{ return m_code; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return m_code; }
	protected:
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
	private:
		wxString m_code;
		int m_state;
		DECLARE_CLASS(FbFilterChildData)
};

class FbFilterTreeModel: public FbTreeModel
{
	public:
		FbFilterTreeModel(const wxSortedArrayString &array, const wxString & first);
		FbFilterTreeModel(const wxString & list, const wxString &sel);
		void SetSel(const wxString &sel);
		wxString GetAll();
		wxString GetSel();
	private:
		DECLARE_CLASS(FbFilterTreeModel)
};

class FbFilterTreeThread: public FbThread
{
	public:
		FbFilterTreeThread(wxEvtHandler * frame, int last_book)
			:FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_last_book(last_book) {}
	protected:
		virtual void * Entry();
	private:
		void Add(wxSortedArrayString &array, const wxString & text);
	private:
		wxEvtHandler * m_frame;
		int m_last_book;
};

#endif // __FBFILTERTREE_H__
