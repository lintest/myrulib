#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/dc.h>
#include "FbTreeItem.h"

#define TREELIST_CASHE_SIZE 128

class FbColumnInfo
{
	public:
		FbColumnInfo(size_t column, int width): m_column(column), m_width(width) {}
		FbColumnInfo(const FbColumnInfo &info): m_column(info.m_column), m_width(info.m_width) {}
		size_t GetColumn() const { return m_column; }
		int GetWidth() const { return m_width; }
	private:
		size_t m_column;
		int m_width;
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbColumnInfo*, FbColumnArray);

class FbTreeModel
{
	public:
		enum Position {
			POS_CHECK,
			POS_PLUS,
			POS_ITEM,
		};

	public:
		FbTreeModel();
		virtual ~FbTreeModel() {}
		virtual size_t GetRowCount() const = 0;
		virtual void DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h) = 0;

		virtual int GoFirstRow() = 0;
		virtual int GoLastRow() = 0;
		virtual int GoNextRow(size_t delta = 1) = 0;
		virtual int GoPriorRow(size_t delta = 1) = 0;

		virtual FbTreeItemId GetCurrent() { return m_current; }
		void SetOwner(wxWindow * owner) { m_owner = owner; };

		virtual FbTreeItemId FindItem(size_t row, bool select) = 0;
		virtual int GetState(const FbTreeItemId &id) = 0;
		virtual wxString GetValue(const FbTreeItemId &id, size_t col) = 0;

	protected:
		const wxBitmap & GetBitmap(const FbTreeItemId &id);

		wxWindow * m_owner;

		FbTreeItemId m_current;

		wxBrush m_normalBrush;
		wxBrush m_hilightBrush;
		wxBrush m_unfocusBrush;

		wxColour m_normalColour;
		wxColour m_hilightColour;
};

class FbTreeModelList: public FbTreeModel
{
	public:
		FbTreeModelList(size_t count = 0);
		virtual ~FbTreeModelList() {};
		virtual size_t GetRowCount() const { return m_count; }
		virtual void SetRowCount(size_t count);

		virtual void DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);

		virtual int GoFirstRow();
		virtual int GoLastRow();
		virtual int GoNextRow(size_t delta = 1);
		virtual int GoPriorRow(size_t delta = 1);

		virtual FbTreeItemId FindItem(size_t row, bool select);
		virtual int GetState(const FbTreeItemId &id) { return -1; }
		virtual wxString GetValue(const FbTreeItemId &id, size_t col);

	private:
		size_t m_count;

};

#endif // __FBTREEMODEL_H__
