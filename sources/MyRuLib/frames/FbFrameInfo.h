#ifndef __FBFRAMEINFO_H__
#define __FBFRAMEINFO_H__

#include <wx/wx.h>
#include <wx/aui/tabmdi.h>
#include "FbMainMenu.h"
#include "FbThread.h"
#include "controls/FbHtmlWindow.h"

class FbFrameInfo
	: public FbHtmlWindow
{
	public:
		static void Execute(wxEvtHandler * owner);
		FbFrameInfo(wxAuiNotebook * parent, bool select);
		void Load(const wxString & html);
		virtual void UpdateFonts(bool refresh = true);
	public:
		void DoEvent(wxEvent& event) {
			GetEventHashTable().HandleEvent(event, this);
		}
	private:
		void OnSave(wxCommandEvent& event);
		DECLARE_CLASS(FbFrameInfo)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEINFO_H__
