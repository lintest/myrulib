#ifndef __FBEXPORTTREE_H__
#define __FBEXPORTTREE_H__

#include "FbTreeModel.h"

class FbExportParentData: public FbParentData
{
	public:
		FbExportParentData(FbModel & model, FbParentData * parent = NULL, const wxString &name = wxEmptyString)
			: FbParentData(model, parent), m_name(name) {}
		virtual bool FullRow(FbModel & model) const
			{ return true; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return m_name; }
		void Sort();
	private:
		wxString m_name;
		DECLARE_CLASS(FbExportParentData)
};

class FbExportChildData: public FbChildData
{
	public:
		FbExportChildData(FbModel & model, FbParentData * parent, int book);
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
	private:
		int m_book;
		wxString m_name;
		wxString m_type;
		int m_size;
		DECLARE_CLASS(FbExportChildData)
};

class FbExportTreeModel: public FbTreeModel
{
	public:
		FbExportTreeModel(const wxString &books, int author = 0);
		wxString GetFormat() const { return m_format; }
		int GetScale() const { return m_scale; }
		void Append(int book, const wxString &filename);
		void Sort();
	private:
		wxString m_format;
		int m_scale;
		DECLARE_CLASS(FbExportTreeModel)
};

#endif // __FBEXPORTTREE_H__