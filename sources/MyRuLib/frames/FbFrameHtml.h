#ifndef __FBFRAMEHTML_H__
#define __FBFRAMEHTML_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbThread.h"
#include "FbWindow.h"
#include "controls/FbHtmlWindow.h"

class FbFrameHtml :
	public wxSplitterWindow
{
	public:
		FbFrameHtml(wxAuiNotebook * parent, int id);
		virtual ~FbFrameHtml();
		void Load(const wxString & html);
		static void Execute();
		virtual void UpdateFonts(bool refresh = true);
		virtual void Localize(bool bUpdateMenu);
	public:
		void DoEvent(wxEvent& event) {
			GetEventHashTable().HandleEvent(event, this);
		}
	private:
		void CreateControls();
		static wxString GetMd5sum(const int id);
		void DoModify();
		void DoSubmit();
		void DoUpdate();
		void ModifyLink(const wxString &key);
		void DeleteLink(const wxString &key);
		void Reset();
	private:
		FbHtmlWindow m_info;
		wxTextCtrl m_Caption;
		wxTextCtrl m_Comment;
		wxToolBar m_ToolBar;
		int m_id;
		wxString m_key;
		wxString m_md5sum;
		FbThread * m_thread;
	private:
		void OnEnter(wxCommandEvent& event);
		void OnSubmit(wxCommandEvent& event);
		void OnModify(wxCommandEvent& event);
		void OnInfoUpdate(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnLinkClicked(wxHtmlLinkEvent& event);
		DECLARE_CLASS(FbFrameHtml)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEHTML_H__
