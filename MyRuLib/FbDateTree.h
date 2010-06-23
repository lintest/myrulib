#ifndef __FBDATETREE_H__
#define __FBDATETREE_H__

#include "FbTreeModel.h"
#include "FbDatabase.h"
#include "FbCollection.h"
#include "FbThread.h"

class FbDateTreeThread: public FbThread
{
	public:
		FbDateTreeThread(wxEvtHandler * frame)
			:FbThread(wxTHREAD_JOINABLE), m_frame(frame) {}
	protected:
		virtual void * Entry();
	private:
		wxEvtHandler * m_frame;
		int m_order;
};

class FbDateYearData: public FbParentData
{
	public:
		FbDateYearData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
	private:
		int m_code;
		DECLARE_CLASS(FbDateMonthData);
};

class FbDateMonthData: public FbParentData
{
	public:
		FbDateMonthData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
	private:
		int m_code;
		DECLARE_CLASS(FbDateMonthData);
};

class FbDateDayData: public FbChildData
{
	public:
		FbDateDayData(FbModel & model, FbParentData * parent, int code, int count)
			: FbChildData(model, parent), m_code(code), m_count(count) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int GetCode() const { return m_code; }
		FbMasterInfo GetInfo() const;
	private:
		int m_code;
		int m_count;
		DECLARE_CLASS(FbDateDayData);
};

#endif // __FBDATETREE_H__
