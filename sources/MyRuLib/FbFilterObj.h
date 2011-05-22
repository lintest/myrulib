#ifndef __FBFILTEROBJ_H__
#define __FBFILTEROBJ_H__

#include <wx/wx.h>

class FbFilterObj: public wxObject
{
	public:
		friend class FbFilterDlg;
	public:
		FbFilterObj();
		FbFilterObj(const FbFilterObj & object);
		FbFilterObj & operator=(const FbFilterObj & object);
		wxString GetSQL() const;
		bool IsEnabled() const { return m_enabled; };
		void Enable(bool value) { m_enabled = value; };
		void Load();
		void Save() const;
	private:
		bool m_enabled;
		bool m_lib;
		bool m_usr;
		bool m_del;
		wxString m_lang;
		wxString m_type;
		DECLARE_CLASS(FbFilterTreeModel)
};

#endif // __FBFILTEROBJ_H__
