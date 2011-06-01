#ifndef __FBEXPORTTREE_H__
#define __FBEXPORTTREE_H__

#include "controls/FbTreeModel.h"
#include <wx/filename.h>
#include <wx/wxsqlite3.h>

class FbConvertArray;

class FbExportParentData: public FbParentData
{
	public:
		FbExportParentData(FbModel & model, FbParentData * parent = NULL, const wxString &name = wxEmptyString)
			: FbParentData(model, parent), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : wxString(); }
		void Sort(FbModel & model);
		FbExportParentData * GetDir(FbModel & model, wxArrayString &dirs);
		void Append(FbModel & model, int book, wxFileName &filename, int size);
		int Compare(const FbExportParentData &data) const;
		void GetFiles(FbModel & model, FbConvertArray & files) const;
	private:
		wxString m_name;
		DECLARE_CLASS(FbExportParentData)
};

class FbExportChildData: public FbChildData
{
	public:
		FbExportChildData(FbModel & model, FbParentData * parent, int book, const wxFileName &filename, int size)
			: FbChildData(model, parent), m_book(book), m_name(filename.GetName()), m_type(filename.GetExt()), m_size(size) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int Compare(const FbExportChildData &data) const;
		wxFileName GetPath(FbModel &model) const;
		virtual int GetBook() const { return m_book; }
	private:
		int m_book;
		wxString m_name;
		wxString m_type;
		int m_size;
		DECLARE_CLASS(FbExportChildData)
};

class FbExportTreeContext
{
	public:
		FbExportTreeContext();
		wxFileName GetFilename(wxSQLite3ResultSet &result);
	private:
		wxString Get(wxSQLite3ResultSet &result, const wxString &field);
		wxString Normalize(const wxString &filename, bool translit = false);
	private:
		bool m_translit_folder;
		bool m_translit_file;
		bool m_underscores;
		int m_digits_count;
		wxString m_template;
};

class FbExportTreeModel: public FbTreeModel
{
	public:
		FbExportTreeModel(const wxString &books, int author = 0);
		void GetFiles(FbConvertArray & files);
		void SetFormat(const wxString & ext, const wxString & arc, int scale = 0)
			{ m_ext = ext;  m_arc = arc; m_scale = scale; }
		const wxString & GetExt() const 
			{ return m_ext; }
		const wxString & GetArc() const 
			{ return m_arc; }
		int GetScale() const 
			{ return m_scale; }
	private:
		wxString m_ext;
		wxString m_arc;
		int m_scale;
		DECLARE_CLASS(FbExportTreeModel)
};

#endif // __FBEXPORTTREE_H__
