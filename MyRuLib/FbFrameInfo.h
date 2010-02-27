#ifndef __FBFRAMEINFO_H__
#define __FBFRAMEINFO_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbWindow.h"
#include "FbFrameHtml.h"

class FbFrameInfo: public FbAuiMDIChildFrame
{
	public:
		FbFrameInfo(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Information"); };
		void Load(const wxString & html);
		static void Execute();
		virtual void UpdateFonts(bool refresh = true);
	protected:
		virtual void CreateControls();
	private:
		FbHtmlWindow m_info;
	private:
		void OnSave(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEINFO_H__
