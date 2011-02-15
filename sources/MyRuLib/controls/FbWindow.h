#ifndef __FBWINDOW_H__
#define __FBWINDOW_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>

class FbDialog
	: public wxDialog
{
	public:
		FbDialog()
			{}

		FbDialog(
			wxWindow* parent,
			wxWindowID id = wxID_ANY,
			const wxString& title = wxEmptyString,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER,
			const wxString& name = wxFrameNameStr
		);

		bool Create(
			wxWindow* parent,
			wxWindowID id = wxID_ANY,
			const wxString& title = wxEmptyString,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER,
			const wxString& name = wxFrameNameStr
		);
	protected:
		void Assign(long winid, int param, bool write);
};

class FbAuiMDIParentFrame
	: public wxAuiMDIParentFrame
{
	public:
		FbAuiMDIParentFrame()
			: m_menubar(NULL) {}

		~FbAuiMDIParentFrame();

		virtual void SetMenuBar(wxMenuBar *pMenuBar)
			{}

	    virtual wxMenuBar * GetMenuBar() const
			{ return m_menubar; }

		void SetMainMenu(wxMenuBar * menubar);

	private:
		wxMenuBar * m_menubar;

};

class FbAuiMDIChildFrame
	: public wxAuiMDIChildFrame
{
	public:
		FbAuiMDIChildFrame(
			wxAuiMDIParentFrame *parent,
            wxWindowID winid,
			const wxString& title,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE,
			const wxString& name = wxFrameNameStr
		);

		bool Create(
			wxAuiMDIParentFrame *parent,
            wxWindowID winid,
			const wxString& title,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE,
			const wxString& name = wxFrameNameStr
		);

		virtual void UpdateFonts(bool refresh = true) {}

		static void UpdateFont(wxHtmlWindow * html, bool refresh);

		virtual void ShowFullScreen(bool show) {}

		virtual wxMenuBar * CreateMenuBar();

		virtual void Localize(bool bUpdateMenu);

	protected:
		void UpdateMenu();

	private:
        void OnActivated(wxActivateEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBWINDOW_H__
