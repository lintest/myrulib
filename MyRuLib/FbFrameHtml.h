#ifndef __FBFRAMEHTML_H__
#define __FBFRAMEHTML_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbBookData.h"
#include "FbWindow.h"
#include "FbHtmlWindow.h"

class FbFrameHtml: public FbAuiMDIChildFrame
{
	public:
		FbFrameHtml(wxAuiMDIParentFrame * parent, int id);
		virtual wxString GetTitle() const { return _("Comments"); };
		void Load(const wxString & html);
		static void Execute();
		virtual void UpdateFonts(bool refresh = true);
		virtual void Localize(bool bUpdateMenu);
	protected:
		virtual void CreateControls();
	private:
		static wxString GetMd5sum(const int id);
		void DoModify();
		void DoSubmit();
		void DoUpdate();
	private:
		FbHtmlWindow m_info;
		wxTextCtrl m_Caption;
		wxTextCtrl m_Comment;
		wxToolBar m_ToolBar;
		int m_id;
		wxString m_key;
		wxString m_md5sum;
	private:
		void OnEnter(wxCommandEvent& event);
		void OnSubmit(wxCommandEvent& event);
		void OnModify(wxCommandEvent& event);
		void OnInfoUpdate(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnLinkClicked(wxHtmlLinkEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEHTML_H__
