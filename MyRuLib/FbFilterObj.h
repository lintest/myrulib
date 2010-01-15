#ifndef __FBFILTEROBJ_H__
#define __FBFILTEROBJ_H__

#include <wx/wx.h>

class FbFilterObj
{
	public:
		FbFilterObj();
		FbFilterObj(const FbFilterObj & obj);
		void Save();
	private:
		bool m_enabled;
		bool m_lib;
		bool m_usr;
		wxString m_lang;
		wxString m_type;
};

#endif // __FBFILTEROBJ_H__
