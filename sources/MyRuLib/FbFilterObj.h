#ifndef __FBFILTEROBJ_H__
#define __FBFILTEROBJ_H__

#include <wx/wx.h>

class FbFilterObj
{
	public:
		friend class FbFilterDlg;
	public:
		FbFilterObj();
		wxString GetSQL();
		bool IsEnabled() { return m_enabled; };
		void Disable() { m_enabled = false; };
	private:
		void Save();
	private:
		bool m_enabled;
		bool m_lib;
		bool m_usr;
		wxString m_lang;
		wxString m_type;
};

#endif // __FBFILTEROBJ_H__
