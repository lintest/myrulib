#ifndef __FBDOWNLIST_H__
#define __FBDOWNLIST_H__

#include "FbTreeModel.h"

class FbDownListData: public FbModelData
{
	public:
		FbDownListData(int code, const wxString &name)
			: m_code(code), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : (wxString)wxEmptyString; }
		int GetCode() const 
			{ return m_code; }
		FbMasterInfo GetInfo() const;
	private:
		int m_code;
		wxString m_name;
		DECLARE_CLASS(FbDownListData);
};

#endif // __FBDOWNLIST_H__
