#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "BooksPanel.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbFrameBaseMenu.h"

class FbFrameBase : public wxAuiMDIChildFrame
{
public:
	FbFrameBase();
	FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& WXUNUSED(name)) { return NULL; };
    bool m_FilterFb2;
    bool m_FilterLib;
    bool m_FilterUsr;
protected:
	virtual void CreateControls() {};
	virtual void UpdateBooklist() = 0;
	int GetModeKey();
	int GetViewKey();
protected:
	void CreateBooksPanel(wxWindow * parent, long substyle);
    BooksPanel m_BooksPanel;
private:
    void OnChangeFilter(wxCommandEvent& event);
	void OnChangeMode(wxCommandEvent& event);
    void OnChangeView(wxCommandEvent & event);
	void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
	void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
	void OnChangeFilterUpdateUI(wxUpdateUIEvent & event);
    void OnExternal(wxCommandEvent& event);
    void OnSubmenu(wxCommandEvent& event);
    void OnAppendBook(FbBookEvent& event);
    void OnAppendAuthor(wxCommandEvent& event);
    void OnAppendSequence(wxCommandEvent& event);
    void OnEmptyBooks(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif //__FBFRAMEBASE_H__

