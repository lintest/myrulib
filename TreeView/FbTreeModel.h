#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/dc.h>

class FbModel;

class FbModelData: public wxObject
{
	public:
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
		virtual size_t GetLevel(FbModel & model) const
			{ return 0; }
		virtual int GetState(FbModel & model) const
			{ return -1; }
		virtual void SetState(FbModel & model, bool state)
			{}
		virtual int Compare(FbModel & model, const FbModelData &data) const
			{ return GetValue(model, 0).CmpNoCase(data.GetValue(model, 0)); }
		virtual size_t Count(FbModel & model) const 
			{ return 0; }
		virtual size_t CountAll(FbModel & model) const 
			{ return 1; }
		virtual FbModelData* Items(FbModel & model, size_t index) const 
			{ return NULL; };
		virtual FbModelData* GetParent(FbModel & model) const 
			{ return NULL; };
	protected:
		static wxString Format(int number);
		DECLARE_CLASS(FbModelData);
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbModelData, FbModelDataArray);

class FbTreeData: public FbModelData
{
	public:
		FbTreeData(): m_parent(NULL) {}
		virtual size_t Count(FbModel & model) const;
		virtual size_t CountAll(FbModel & model) const;
		virtual FbModelData* Items(FbModel & model, size_t index) const;
		virtual int GetState(FbModel & model) const;
		virtual void SetState(FbModel & model, bool state);
		virtual size_t GetLevel(FbModel & model) const;
		virtual FbModelData* GetParent(FbModel & model) const;
	public:
		void Add(FbModel & model, FbModelData* data);
		void CheckState(FbModel & model);
	protected:
		virtual void SetState(int state) {};
		virtual int GetState() const { return -1; };
	private:
		FbModelDataArray m_items;
		FbTreeData * m_parent;
		DECLARE_CLASS(FbTreeData);
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
	public:
		FbModel();
		virtual ~FbModel() {}

		void DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
		void SetFocused(bool focused) { m_focused = focused; }

		virtual int GoFirstRow() = 0;
		virtual int GoLastRow() = 0;
		virtual int GoNextRow(size_t delta = 1) = 0;
		virtual int GoPriorRow(size_t delta = 1) = 0;

		virtual size_t GetPosition() { return m_position; }
		void SetOwner(wxWindow * owner) { m_owner = owner; };

		virtual size_t FindRow(size_t row, bool select) = 0;
		virtual FbModelData * GetData(size_t row) = 0;
		virtual FbModelData * GetCurrent() = 0;
		virtual size_t GetRowCount() const = 0;

	protected:
		const wxBitmap & GetBitmap(int state);
		virtual void DoDrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h) = 0;
		virtual void DrawItem(FbModelData &data, wxDC &dc, const wxRect &rect, const FbColumnArray &cols);

		wxWindow * m_owner;

		size_t m_position;

		wxBrush m_normalBrush;
		wxBrush m_hilightBrush;
		wxBrush m_unfocusBrush;

		wxColour m_normalColour;
		wxColour m_hilightColour;

		wxFont m_normalFont;
		wxFont m_boldFont;

		wxPen m_borderPen;

		bool m_focused;

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
	public:
		virtual void Append(FbModelData * data) = 0;
		virtual void Replace(FbModelData * data) = 0;
		virtual void Delete() = 0;
	protected:
		virtual void DoDrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
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
		virtual FbModelData * GetData(size_t row)
			{ return row && row <= m_list.Count() ? &m_list[row - 1] : NULL; }
		virtual FbModelData * GetCurrent()
			{ return GetData(m_position); };
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

		virtual int GoFirstRow();
		virtual int GoLastRow();
		virtual int GoNextRow(size_t delta = 1);
		virtual int GoPriorRow(size_t delta = 1);

		virtual size_t FindRow(size_t row, bool select) = 0;
		virtual FbModelData * GetData(size_t row) = 0;
		virtual FbModelData * GetCurrent() = 0;

		virtual size_t GetRowCount() 
			{ return m_root ? m_root->CountAll(*this) : 0; }

	protected:
		virtual void DoDrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);
		void DoDrawItem(FbModelData &data, wxDC &dc, const wxRect &rect, const FbColumnArray &cols, int h, int &y);
		FbModelData * GetLast(FbModelData &data);

	protected:
		FbModelData * m_root;
		FbModelData * m_current;
		DECLARE_CLASS(FbTreeModel);
};

#endif // __FBTREEMODEL_H__
