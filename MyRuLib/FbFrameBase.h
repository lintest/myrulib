#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbBookPanel.h"
#include "FbBookEvent.h"
#include "FbParams.h"

class FbFolderData: public wxTreeItemData
{
	public:
		FbFolderData(const int id, const FbFolderType type = FT_FOLDER)
			: m_id(id), m_type(type) {};
		const int GetId() { return m_id; };
		const FbFolderType GetType() { return m_type; };
	private:
		int m_id;
		FbFolderType m_type;
};

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
	FbBookPanel * GetBookPanel() { return &m_BooksPanel; };
protected:
	virtual void CreateControls() {};
	virtual void UpdateBooklist() = 0;
	int GetModeKey();
	int GetViewKey();
protected:
	void CreateBooksPanel(wxWindow * parent, long substyle);
	FbBookPanel m_BooksPanel;
private:
	void OnActivated(wxActivateEvent & event);
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
	void OnTreeCollapsing(wxTreeEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif //__FBFRAMEBASE_H__

