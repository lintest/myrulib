#ifndef __FBDOWNLIST_H__
#define __FBDOWNLIST_H__

#include "controls/FbTreeModel.h"
#include "FbMasterTypes.h"

class FbDownListData: public FbModelData
{
	public:
		FbDownListData(FbMasterDownInfo::DownType code, const wxString &name)
			: m_code(code), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : (wxString)wxEmptyString; }
		FbMasterDownInfo::DownType GetCode() const
			{ return m_code; }
		FbMasterInfo GetInfo(FbModel & model) const;
	private:
		FbMasterDownInfo::DownType m_code;
		wxString m_name;
		DECLARE_CLASS(FbDownListData);
};

class FbDownList: public FbListModel
{
	public:
		virtual void Delete();
		virtual size_t GetRowCount() const;
	protected:
		virtual FbModelItem DoGetData(size_t row, int &level);
	protected:
		DECLARE_CLASS(FbDownList);
};

#endif // __FBDOWNLIST_H__
