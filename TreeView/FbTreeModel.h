#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/dc.h>
#include <wx/dynarray.h>

WX_DEFINE_ARRAY_SIZE_T(size_t, FbArraySizeT);

class FbModel;

class FbModelData: public wxObject
{
	public:
		static wxString Format(int number);

		FbModelData()
			{}
		virtual ~FbModelData()
			{}
		virtual wxString GetValue(FbModel & model, size_t col) const
			{ return wxEmptyString; }
		virtual void SetValue(FbModel & model, size_t col, const wxString &name)
			{}
		virtual bool FullRow(FbModel & model) const
			{ return false; }
		virtual bool IsBold(FbModel & model) const
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
	public:
		int GetState(FbModel & model) const;
		void SetState(FbModel & model, bool state);
	protected:
		virtual void DoSetState(FbModel & model, int state) {};
		virtual int DoGetState(FbModel & model) const { return -1; };
		void CheckState(FbModel & model);
		DECLARE_CLASS(FbModelData);
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbModelData, FbModelDataArray);

class FbParentData: public FbModelData
{
	public:
		FbParentData(FbModel & model, FbParentData * parent = 0);
		virtual size_t Count(FbModel & model) const
			{ return m_items.Count(); }
		virtual FbModelData* GetParent(FbModel & model) const
			{ return m_parent; }
		virtual bool IsBold(FbModel & model) const
			{ return true; }
		void Delete(size_t index)
			{ m_items.RemoveAt(index); }
		virtual size_t CountAll(const FbModel & model) const;
		virtual FbModelData* Items(FbModel & model, size_t index) const;
	private:
		void Add(FbModel & model, FbModelData* data);
		FbModelDataArray m_items;
		FbParentData * m_parent;
		DECLARE_CLASS(FbChildData);
		friend class FbChildData;
};

class FbChildData: public FbModelData
{
	public:
		FbChildData(FbModel & model, FbParentData * parent = 0);
		virtual FbModelData* GetParent(FbModel & model) const
			{ return m_parent; }
	private:
		FbParentData * m_parent;
		bool m_selected;
		DECLARE_CLASS(FbChildData);
};

class FbColumnInfo: public wxObject
{
	public:
		FbColumnInfo(size_t column, int width, int alignment)
			: m_column(column), m_width(width), m_alignment(alignment) {}
		FbColumnInfo(const FbColumnInfo &info)
			: m_column(info.m_column), m_width(info.m_width), m_alignment(info.m_alignment) {}
		size_t GetColumn() const { return m_column; }
		int GetWidth() const { return m_width; }
		int GetAlignment() const { return m_alignment; }
	private:
		size_t m_column;
		int m_width;
		int m_alignment;
		DECLARE_CLASS(FbColumnInfo);
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbColumnInfo, FbColumnArray);

class FbModel: public wxObject
{
	protected:
		class PaintContext{
			public:
				PaintContext(wxDC &dc);
				wxBrush m_normalBrush;
				wxBrush m_hilightBrush;
				wxBrush m_unfocusBrush;
				wxColour m_normalColour;
				wxColour m_hilightColour;
				wxFont m_normalFont;
				wxFont m_boldFont;
				wxPen m_borderPen;
				bool m_current;
				bool m_selected;
				bool m_hidden;
				int m_level;
		};
	public:
		FbModel();
		virtual ~FbModel() {}

		void DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
		void SetFocused(bool focused)
			{ m_focused = focused; }
		FbModelData * GetData(size_t row)
			{ int level; return DoGetData(row, level); }
		FbModelData * GetData(size_t row, int &level)
			{ return DoGetData(row, level); }

		virtual int GoFirstRow() = 0;
		virtual int GoLastRow() = 0;
		virtual int GoNextRow(size_t delta = 1) = 0;
		virtual int GoPriorRow(size_t delta = 1) = 0;

		virtual size_t GetPosition() { return m_position; }
		void SetOwner(wxWindow * owner) { m_owner = owner; };

		virtual size_t FindRow(size_t row, bool select) = 0;
		virtual size_t GetRowCount() const = 0;

		virtual void Append(FbModelData * data) = 0;
		virtual void Replace(FbModelData * data) = 0;
		virtual void Delete() = 0;

		virtual FbModelData * GetCurrent()
			{ return GetData(m_position); }

	protected:
		const wxBitmap & GetBitmap(int state);
		void DrawItem(FbModelData &data, wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols);
		virtual void DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h) = 0;
		virtual FbModelData * DoGetData(size_t row, int &level) = 0;
		bool IsSelected(size_t row);

	protected:
		wxWindow * m_owner;
		size_t m_position;
		bool m_focused;
		size_t m_shift;
		FbArraySizeT m_ctrls;
		DECLARE_CLASS(FbModel);
		friend class FbTreeViewMainWindow;
};

class FbListModel: public FbModel
{
	public:
		virtual int GoFirstRow();
		virtual int GoLastRow();
		virtual int GoNextRow(size_t delta = 1);
		virtual int GoPriorRow(size_t delta = 1);
		virtual size_t FindRow(size_t row, bool select);
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
		virtual FbModelData * DoGetData(size_t row, int &level)
			{ level = 0; return row && row <= m_list.Count() ? &m_list[row - 1] : NULL; }
	private:
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
		FbModelData * GetRoot()
			{ return m_root; }

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

	protected:
		bool DoDelete(FbModelData &parent, size_t &row);
		virtual FbModelData * DoGetData(size_t row, int &level);
		virtual void DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
		void DrawTreeItem(FbModelData &data, wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, int h, size_t &row);
		FbModelData * FindData(FbModelData &parent, size_t &row, int &level);
		FbModelData * GetLast(FbModelData &parent);

	protected:
		FbModelData * m_root;
		DECLARE_CLASS(FbTreeModel);
};

#endif // __FBTREEMODEL_H__
