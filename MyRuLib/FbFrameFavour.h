#ifndef __FBFRAMEFAVOUR_H__
#define __FBFRAMEFAVOUR_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "wx/treelistctrl.h"
#include "FbFrameBase.h"
#include "FbTreeListCtrl.h"
#include "FbBookEvent.h"

enum FbFolderType {
	FT_FOLDER,
	FT_RATING,
	FT_DOWNLOAD,
};

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

class FbFrameFavour : public FbFrameBase
{
public:
	FbFrameFavour(wxAuiMDIParentFrame * parent);
	void UpdateFolder(const int iFolder, const FbFolderType type);
protected:
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
	virtual void CreateControls();
	virtual void UpdateBooklist();
private:
    void CreateBookInfo();
	void FillFolders(const int iCurrent = 0);
	void FillByFolder(FbFolderData * data);
	void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
	FbFolderData * GetSelected();
private:
    FbTreeListCtrl * m_FolderList;
private:
    void OnFavoritesDel(wxCommandEvent & event);
    void OnFolderAppend(wxCommandEvent & event);
    void OnFolderModify(wxCommandEvent & event);
    void OnFolderDelete(wxCommandEvent & event);
    void OnFolderSelected(wxTreeEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEFAVOUR_H__
