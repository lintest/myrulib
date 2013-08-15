#ifndef __FBDATETREE_H__
#define __FBDATETREE_H__

#include "wx/FbSQLite3.h"
#include "controls/FbTreeModel.h"
#include "FbDatabase.h"
#include "FbCollection.h"
#include "FbFrameThread.h"

class FbDateTreeThread : public FbFrameThread
{
	public:
		FbDateTreeThread(wxEvtHandler * frame, const wxString &counter)
			: FbFrameThread(frame, counter) {}
	protected:
		virtual void * Entry();
		void MakeModel(FbSQLite3ResultSet &result);
};

class FbDateYearData: public FbParentData
{
	public:
		FbDateYearData(FbModel & model, FbParentData * parent, int code)
			: FbParentData(model, parent), m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int GetCode() const { return m_code; }
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
		int GetCode() const { return m_code; }
	private:
		int m_code;
		DECLARE_CLASS(FbDateMonthData);
};

class FbDateDayData: public FbChildData
{
	public:
		FbDateDayData(FbModel & model, FbParentData * parent, int code);
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int GetCode() const { return m_code; }
		FbMasterInfo GetInfo(FbModel & model) const;
		bool operator==(const FbMasterInfo & info) const;
		void SetCount(int count) { m_count = count; }
	private:
		int m_code;
		int m_count;
		DECLARE_CLASS(FbDateDayData);
};

class FbDateTreeModel: public FbTreeModel
{
	public:
		void SetCounter(const wxString & filename);
		int GetCount(int code);
	private:
		FbIntegerHash m_counter;
		FbSQLite3Database m_database;
		DECLARE_CLASS(FbDateTreeModel);
};

#endif // __FBDATETREE_H__
