#ifndef __FBFRAMEINFO_H__
#define __FBFRAMEINFO_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbWindow.h"
#include "FbFrameHtml.h"
#include "FbMainMenu.h"

class FbFrameInfo: public FbAuiMDIChildFrame
{
	public:
		FbFrameInfo(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Information"); };
		void Load(const wxString & html);
		static void Execute(wxEvtHandler * owner);
		virtual void UpdateFonts(bool refresh = true);
		virtual wxMenuBar * CreateMenuBar();
	protected:
		virtual void CreateControls();
	private:
		FbHtmlWindow m_info;
	private:
		void OnSave(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameInfo)
	private:
		class MainMenu: public FbMenuBar
		{
			public:
				MainMenu();
			protected:
				class MenuFile: public FbMenu {
					public: MenuFile();
				};
		};
};

#endif // __FBFRAMEINFO_H__
