#ifndef __FBFRAMEFOLDER_H__
#define __FBFRAMEFOLDER_H__

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

class FbFolderParentData: public FbParentData
{
	public:
		FbFolderParentData(FbModel & model, FbParentData * parent, const wxString &name)
			: FbParentData(model, parent), m_name(name) {}
		virtual wxString GetValue(FbModel & model, size_t col) const
			{ return col ? (wxString)wxEmptyString : m_name; }
		virtual bool IsBold(FbModel & model) const
			{ return true; }
	private:
		wxString m_name;
		DECLARE_CLASS(FbGenreParentData);
};

class FbFolderChildData: public FbChildData
{
	public:
		FbFolderChildData(FbModel & model, FbParentData * parent, int code, const wxString &name, FbFolderType type)
			: FbChildData(model, parent), m_code(code), m_name(name), m_type(type) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 0 ? m_name : (wxString)wxEmptyString; }
		int GetCode() const 
			{ return m_code; }
		FbFolderType GetType() const 
			{ return m_type; }
		void SetName(const wxString &name) 
			{ m_name = name; }
	private:
		int m_code;
		wxString m_name;
		FbFolderType m_type;
		DECLARE_CLASS(FbDownListData);
};

class FbFrameFolder : public FbFrameBase
{
	public:
		FbFrameFolder(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("My folders"); };
		void UpdateFolder(const int iFolder, const FbFolderType type);
		virtual void ShowFullScreen(bool show);
		void Localize(bool bUpdateMenu);
	protected:
		virtual void CreateControls();
		virtual wxToolBar * CreateToolBar(long style = wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxWindowID winid = wxID_ANY, const wxString& name = wxEmptyString);
	private:
		void CreateColumns();
		void CreateBookInfo();
		void FillFolders(const int iCurrent = 0);
		void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
		FbParentData * m_folders;
	private:
		void OnFavoritesDel(wxCommandEvent & event);
		void OnFolderAppend(wxCommandEvent & event);
		void OnFolderModify(wxCommandEvent & event);
		void OnFolderDelete(wxCommandEvent & event);
		void OnFolderSelected(wxTreeEvent & event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameFolder)
};

#endif // __FBFRAMEFOLDER_H__
