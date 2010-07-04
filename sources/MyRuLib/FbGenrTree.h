#ifndef __FBGENRTREE_H__
#define __FBGENRTREE_H__

#include "FbTreeModel.h"

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
			: FbChildData(model, parent), m_code(code), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col) const
			{ return col ? (wxString)wxEmptyString : m_name; }
		wxString GetCode() const
			{ return m_code; }
		FbMasterInfo GetInfo() const;
	private:
		wxString m_code;
		wxString m_name;
		DECLARE_CLASS(FbGenrChildData);
};

#endif // __FBGENRTREE_H__

