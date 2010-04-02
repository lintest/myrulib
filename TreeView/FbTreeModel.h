#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/dc.h>

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
WX_DECLARE_OBJARRAY(FbColumnInfo, FbColumnArray);

class FbTreeModel
{
	public:
		FbTreeModel() {}
		virtual ~FbTreeModel() {}
		virtual size_t GetRowCount() const = 0;
		virtual void Draw(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h) = 0;

	protected:
		const wxBitmap & GetBitmap(int state) const;

};

class FbTreeModelList: public FbTreeModel
{
	public:
		FbTreeModelList(size_t count): m_count(count) {}
		virtual ~FbTreeModelList() {};
		virtual size_t GetRowCount() const { return m_count; }
		virtual void Draw(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h);

	private:
		size_t m_count;

};

#endif // __FBTREEMODEL_H__
