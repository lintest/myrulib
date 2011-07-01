#ifndef __FBFLDRTREE_H__
#define __FBFLDRTREE_H__

#include "controls/FbTreeModel.h"

class FbFolderParentData: public FbParentData
{
	public:
		FbFolderParentData(FbModel & model, FbParentData * parent, const wxString &name)
			: FbParentData(model, parent), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col) const
			{ return col ? (wxString)wxEmptyString : m_name; }
		virtual bool IsBold(FbModel & model) const
			{ return true; }
	private:
		wxString m_name;
		DECLARE_CLASS(FbFolderParentData);
};

class FbFolderChildData: public FbChildData
{
	public:
		FbFolderChildData(FbModel & model, FbParentData * parent, int code, const wxString &name)
			: FbChildData(model, parent), m_code(code), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : (wxString)wxEmptyString; }
		int GetCode() const
			{ return m_code; }
		void SetName(const wxString &name)
			{ m_name = name; }
		FbMasterInfo GetInfo() const;
	private:
		int m_code;
		wxString m_name;
		DECLARE_CLASS(FbFolderChildData);
};

class FbCommChildData: public FbChildData
{
	public:
		FbCommChildData(FbModel & model, FbParentData * parent);
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : wxString(); }
		FbMasterInfo GetInfo() const;
	private:
		wxString m_name;
		DECLARE_CLASS(FbCommChildData);
};

class FbRateChildData: public FbChildData
{
	public:
		FbRateChildData(FbModel & model, FbParentData * parent, int code);
		int GetCode() const
			{ return m_code; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : wxString(); }
		FbMasterInfo GetInfo() const;
	private:
		int m_code;
		wxString m_name;
		DECLARE_CLASS(FbCommChildData);
};

#endif // __FBFLDRTREE_H__
