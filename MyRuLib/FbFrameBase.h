#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbBookPanel.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbWindow.h"

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

class FbFrameBase : public FbAuiMDIChildFrame
{
public:
	FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
	bool m_FilterFb2;
	bool m_FilterLib;
	bool m_FilterUsr;
	void UpdateInfo(int id);
	virtual void UpdateFonts(bool refresh = true);
	wxString GetOrderSQL() { return m_BooksPanel->GetOrderSQL(); };
	void UpdateStatus();
protected:
	virtual void CreateControls();
	virtual void UpdateBooklist() = 0;
	virtual wxString GetStatus();
	int GetModeKey();
	int GetViewKey();
	void OnSubmenu(wxCommandEvent& event);
	void CreateBooksPanel(wxWindow * parent, long substyle);
	int GetColOrder(int col);
	wxString Naming(int count, const wxString &single, const wxString &genitive, const wxString &plural);
protected:
	FbTreeListCtrl * m_MasterList;
	FbBookPanel * m_BooksPanel;
	int m_BooksCount;
private:
	void OnActivated(wxActivateEvent & event);
	void OnBooksCount(wxCommandEvent& event);
	void OnDirection(wxCommandEvent& event);
	void OnChangeOrder(wxCommandEvent& event);
	void OnChangeFilter(wxCommandEvent& event);
	void OnChangeMode(wxCommandEvent& event);
	void OnChangeView(wxCommandEvent & event);
	void OnColClick(wxListEvent& event);
	void OnMenuOrderUpdateUI(wxUpdateUIEvent & event);
	void OnDirectionUpdateUI(wxUpdateUIEvent & event);
	void OnChangeOrderUpdateUI(wxUpdateUIEvent & event);
	void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
	void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
	void OnChangeFilterUpdateUI(wxUpdateUIEvent & event);
	void OnExternal(wxCommandEvent& event);
	void OnAppendBook(FbBookEvent& event);
	void OnAppendAuthor(wxCommandEvent& event);
	void OnAppendSequence(wxCommandEvent& event);
	void OnEmptyBooks(wxCommandEvent& event);
	void OnTreeCollapsing(wxTreeEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif //__FBFRAMEBASE_H__

