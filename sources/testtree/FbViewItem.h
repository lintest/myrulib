#ifndef __FBVIEWITEM_H__
#define __FBVIEWITEM_H__

#include <wx/wx.h>

class FbViewItem: public wxObject
{
	public:
		enum Types {
			None,
			Auth,
			Book,
		};
		FbViewItem(Types type = None, int code = 0)
			: m_type(type), m_code(code) {}
		FbViewItem(const FbViewItem & item)
			: m_type(item.m_type), m_code(item.m_code) {}
		FbViewItem & operator=(const FbViewItem & item)
			{ m_type = item.m_type; m_code = item.m_code; return *this; }
		operator bool() const
			{ return m_type != None && m_code; }
		Types GetType() const
			{ return m_type; }
		int GetCode() const
			{ return m_code; }
	private:
		Types m_type;
		int m_code;
		DECLARE_CLASS(FbViewItem);
};

#endif // __FBVIEWITEM_H__
