#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/dc.h>
#include <wx/dynarray.h>
#include "FbViewItem.h"

class FbModel;

class FbMasterInfo;

class FbModelData;

class FbModelData: public wxObject
{
	public:
		static wxString Format(int number);

		FbModelData()
			{}
		virtual ~FbModelData()
			{}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return wxEmptyString; }
		virtual void SetValue(FbModel & model, size_t col, const wxString &name)
			{}
		virtual bool FullRow(FbModel & model) const
			{ return false; }
		virtual bool IsBold(FbModel & model) const
			{ return false; }
		virtual bool IsGray(FbModel & model) const
			{ return false; }
		virtual int Compare(FbModel & model, const FbModelData &data) const
			{ return GetValue(model, 0).CmpNoCase(data.GetValue(model, 0)); }
		virtual size_t Count(FbModel & model) const
			{ return 0; }
		virtual size_t CountAll(const FbModel & model) const
			{ return 1; }
		virtual FbModelData* Items(FbModel & model, size_t index) const
			{ return NULL; };
		virtual FbModelData* GetParent(FbModel & model) const
			{ return NULL; };
		virtual bool HiddenRoot() const
			{ return true; }
		virtual FbModelData * Clone() const
			{ return NULL; }
		virtual int GetType() const
			{ return 0; }
		virtual bool HasChildren(FbModel & model) const
			{ return 0; }
		virtual bool IsExpanded(FbModel & model) const
			{ return false; }
		virtual bool Expand(FbModel & model, bool expand) 
			{ return false; }
	public:
#ifdef _MYRULIB
		virtual FbMasterInfo GetInfo() const;
		virtual FbViewItem GetView() const { return FbViewItem::None; }
#endif // _MYRULIB
		virtual int GetBook() const { return 0; }
	public:
		int GetState(FbModel & model) const;
		void SetState(FbModel & model, bool state);
		void CheckState(FbModel & model);
	protected:
		virtual void DoSetState(FbModel & model, int state) {}
		virtual int DoGetState(FbModel & model) const { return 0; }
		DECLARE_CLASS(FbModelData);
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbModelData, FbModelDataArray);

class FbModelItem: public wxObject
{
	public:
		FbModelItem()
			: m_model(NULL), m_data(NULL), m_virual(false) {}
		FbModelItem(FbModel & model, FbModelData * data = NULL)
			: m_model(&model), m_data(data ? data->Clone() : data), m_virual(false)
				{ if (m_data == NULL) m_data = data; else m_virual = true; }
		FbModelItem(const FbModelItem &item)
			: m_model(item.m_model), m_data(item.m_data ? item.m_data->Clone() : item.m_data), m_virual(false)
				{ if (m_data == NULL) m_data = item.m_data; else m_virual = true; }
		~FbModelItem()
			{ if (m_virual) wxDELETE(m_data); }
		wxString operator[](size_t col) const
			{ return m_data ? m_data->GetValue(*m_model, col) : wxString(); }
		inline operator bool() const
			{ return m_data != NULL; }
		inline FbModelData * operator&() const
			{ return m_data; }
		inline operator wxString() const
			{ return operator[](0); }
		FbModelItem & operator =(const FbModelItem &item);
	public:
		// Use this functions only for MyRuLib application
		FbMasterInfo GetInfo() const;
	public:
		size_t Count()
			{ return m_data ? m_data->Count(*m_model) : 0; }
		size_t CountAll()
			{ return m_data ? m_data->CountAll(*m_model) : 0; }
		bool FullRow()
			{ return m_data ? m_data->FullRow(*m_model) : false; }
		bool IsBold()
			{ return m_data ? m_data->IsBold(*m_model) : false; }
		bool IsGray()
			{ return m_data ? m_data->IsGray(*m_model) : false; }
		FbModelItem Items(size_t index)
			{ return m_data ? FbModelItem(*m_model, m_data->Items(*m_model, index)) : FbModelItem(); }
		FbModelItem GetParent()
			{ return m_data ? FbModelItem(*m_model, m_data->GetParent(*m_model)) : FbModelItem(); }
		void SetValue(size_t col, const wxString &name)
			{ if (m_data) m_data->SetValue(*m_model, col, name); }
		int GetState() const
			{ return m_data ? m_data->GetState(*m_model) : 0; }
		void SetState(bool state)
			{ if (m_data) m_data->SetState(*m_model, state); }
		int GetBook() const 
			{ return m_data ? m_data->GetBook() : 0; }
		bool HasChildren(FbModel & model) const
			{ return m_data ? m_data->HasChildren(model) : false; }
		bool IsExpanded() const
			{ return m_data ? m_data->IsExpanded(*m_model) : false; }
		bool Expand(bool expand) 
			{ return m_data ? m_data->Expand(*m_model, expand) : false; }
	private:
		FbModel * m_model;
		FbModelData * m_data;
		bool m_virual;
		DECLARE_CLASS(FbModelItem);
};

class FbParentData: public FbModelData
{
	public:
		FbParentData(FbModel & model, FbParentData * parent = NULL);
		virtual size_t Count(FbModel & model) const
			{ return m_items.Count(); }
		virtual FbModelData * GetParent(FbModel & model) const
			{ return m_parent; }
		FbParentData * GetParent() const
			{ return m_parent; }
		void SetParent(FbParentData * parent)
			{ if ((m_parent = parent) != NULL) parent->m_items.Add(this); }
		virtual bool IsBold(FbModel & model) const
			{ return true; }
		void Delete(size_t index)
			{ m_items.RemoveAt(index); }
		virtual size_t CountAll(const FbModel & model) const;
		virtual FbModelData* Items(FbModel & model, size_t index) const;
	protected:
		FbModelDataArray m_items;
	private:
		void Add(FbModel & model, FbModelData* data);
		FbParentData * m_parent;
		DECLARE_CLASS(FbChildData);
		friend class FbChildData;
};

class FbChildData: public FbModelData
{
	public:
		FbChildData(FbModel & model, FbParentData * parent = NULL);
		virtual FbModelData* GetParent(FbModel & model) const
			{ return m_parent; }
		FbParentData * GetParent() const
			{ return m_parent; }
	private:
		FbParentData * m_parent;
		bool m_selected;
		DECLARE_CLASS(FbChildData);
};

class FbColumnInfo: public wxObject
{
	public:
		FbColumnInfo(size_t column, int width, int alignment, int fixed = 0)
			: m_column(column), m_width(width), m_alignment(alignment), m_fixed(fixed) {}
		FbColumnInfo(const FbColumnInfo &info)
			: m_column(info.m_column), m_width(info.m_width), m_alignment(info.m_alignment), m_fixed(info.m_fixed) {}
		size_t GetColumn() const { return m_column; }
		int GetWidth() const { return m_width; }
		int GetAlignment() const { return m_alignment; }
		int GetFixed() const { return m_fixed; }
	private:
		size_t m_column;
		int m_width;
		int m_alignment;
		int m_fixed;
		DECLARE_CLASS(FbColumnInfo);
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbColumnInfo, FbColumnArray);

WX_DEFINE_SORTED_ARRAY_SIZE_T(size_t, FbSortedArraySizeT);

#include <wx/hashmap.h>
WX_DECLARE_HASH_MAP( int, int, wxIntegerHash, wxIntegerEqual, FbIntegerHash);

class FbModel: public wxObject
{
	protected:
		class PaintContext{
			public:
				PaintContext(FbModel &mode, wxDC &dc);
				wxWindow * m_window;
				wxBrush m_normalBrush;
				wxBrush m_hilightBrush;
				wxBrush m_unfocusBrush;
				wxColour m_normalColour;
				wxColour m_hilightColour;
				wxColour m_graytextColour;
				wxFont m_normalFont;
				wxFont m_boldFont;
				wxPen m_borderPen;
				bool m_directory;
				bool m_current;
				bool m_selected;
				bool m_multuply;
				bool m_checkbox;
				bool m_vrules;
				bool m_hrules;
				bool m_hidden;
				int m_level;
		};
	public:
		FbModel();
		virtual ~FbModel() {}

		void DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);

		void SetFocused(bool focused)
			{ m_focused = focused; }
		FbModelItem GetData(size_t row)
			{ int level; return DoGetData(row, level); }
		FbModelItem GetData(size_t row, int &level)
			{ return DoGetData(row, level); }

		virtual int GoFirstRow() = 0;
		virtual int GoLastRow() = 0;
		virtual int GoNextRow(size_t delta = 1) = 0;
		virtual int GoPriorRow(size_t delta = 1) = 0;

		virtual size_t GetPosition() { return m_position; }
		void SetOwner(wxWindow * owner) { m_owner = owner; };
		wxWindow * GetOwner() { return m_owner; };

		virtual size_t FindRow(size_t row, bool select) = 0;
		virtual size_t GetRowCount() const = 0;

		virtual void Append(FbModelData * data) = 0;
		virtual void Replace(FbModelData * data) = 0;
		virtual void Delete() = 0;

		virtual void SingleCheck(size_t row = 0);
		virtual void MultiplyCheck() {}

		virtual void SelectAll(bool value) {}

		virtual wxString GetText(wxArrayInt &columns) 
			{ return wxEmptyString; }

		virtual FbModelItem GetCurrent()
			{ return GetData(m_position); }

		void SetShift(bool select);
		void InitCtrls();
		void InvertCtrl();

	public:
		// Use this functions only for MyRuLib application
		virtual size_t GetSelected(wxArrayInt &items) { return 0; }
		virtual int GetBook() { return 0; }
		virtual FbViewItem GetView() { return FbViewItem::None; }

	protected:
		const wxBitmap & GetBitmap(int state);
		void DrawItem(FbModelItem &data, wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols);
		void DrawButton(wxWindow * window, wxDC &dc, wxRect &rect, bool expand);
		virtual void DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h) = 0;
		virtual FbModelItem DoGetData(size_t row, int &level) = 0;
		bool IsSelected(size_t row);

	protected:
		wxWindow * m_owner;
		size_t m_position;
		bool m_focused;
		size_t m_shift;
		FbSortedArraySizeT m_ctrls;
		DECLARE_CLASS(FbModel);
};

class FbListModel: public FbModel
{
	public:
		virtual int GoFirstRow();
		virtual int GoLastRow();
		virtual int GoNextRow(size_t delta = 1);
		virtual int GoPriorRow(size_t delta = 1);
		virtual size_t FindRow(size_t row, bool select);
		virtual void MultiplyCheck();
	protected:
		virtual void DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
		DECLARE_CLASS(FbListModel);
};

class FbListStore: public FbListModel
{
	public:
		virtual void Append(FbModelData * data);
		virtual void Replace(FbModelData * data);
		virtual void Delete();
	public:
		virtual size_t GetRowCount() const
			{ return m_list.Count(); }
	protected:
		virtual FbModelItem DoGetData(size_t row, int &level)
			{ level = 0; return row && row <= m_list.Count() ? FbModelItem(*this, &m_list[row - 1]) : *this; }
	protected:
		FbModelDataArray m_list;
		DECLARE_CLASS(FbListStore);
};

class FbTreeModel: public FbModel
{
	public:
		FbTreeModel()
			: m_root(NULL) {}
		virtual ~FbTreeModel()
			{ wxDELETE(m_root); }
		FbModelItem GetRoot()
			{ return FbModelItem(*this, m_root); }

		void SetRoot(FbModelData * root);

		virtual int GoFirstRow();
		virtual int GoLastRow();
		virtual int GoNextRow(size_t delta = 1);
		virtual int GoPriorRow(size_t delta = 1);

		virtual size_t FindRow(size_t row, bool select);
		virtual size_t GetRowCount() const;

		virtual void Append(FbModelData * data) {}
		virtual void Replace(FbModelData * data) {}
		virtual void Delete();

		virtual void MultiplyCheck();
		virtual void SelectAll(bool value);

	protected:
		bool DoDelete(FbModelData &parent, size_t &row);
		virtual FbModelItem DoGetData(size_t row, int &level);
		virtual void DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
		void DrawTreeItem(FbModelItem &data, wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, int h, size_t &row);
		FbModelItem FindData(FbModelItem &parent, size_t &row, int &level);
		FbModelItem GetLast(FbModelItem &parent);
		void DoCheck(FbModelItem &parent, size_t max, size_t &row, int &state);
		void DoSelect(FbModelItem &parent, bool value);

	protected:
		FbModelData * m_root;
		DECLARE_CLASS(FbTreeModel);
};

#endif // __FBTREEMODEL_H__
