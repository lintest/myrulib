#ifndef __FBFILTERDLG_H__
#define __FBFILTERDLG_H__

#include <wx/wx.h>
#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/treectrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include "FbWindow.h"
#include "FbTreeListCtrl.h"
#include "FbFilterObj.h"
#include "FbDatabase.h"
#include "FbCheckList.h"
#include "FbParams.h"

///////////////////////////////////////////////////////////////////////////////
/// Class FbFilterList
///////////////////////////////////////////////////////////////////////////
class FbFilterList: public FbCheckList
{
	public:
		FbFilterList(wxWindow *parent, wxWindowID id, const wxString &title);
		wxString Load(FbDatabase & database, const wxString & sql, const wxString & filter);
		void Read(const wxString & filter, int key);
		int Append(wxTreeItemId parent, wxString &text, const wxString & filter);
		wxString GetValue();
};

///////////////////////////////////////////////////////////////////////////////
/// Class FbFilterDlg
///////////////////////////////////////////////////////////////////////////////
class FbFilterDlg : public FbDialog
{
	public:
		FbFilterDlg(FbFilterObj & filter);
		static bool Execute(FbFilterObj & filter);
	private:
		FbTreeListCtrl * CreateTree(const wxString & title);
		void Assign(FbFilterObj & filter);
		void FillTree(FbDatabase & database, FbTreeListCtrl* treelist, const wxString & sql, const wxString & filter);

	private:
		enum
		{
			ID_CHECK_LIB = 1000,
			ID_CHECK_USR,
			ID_TREE_LANG,
			ID_TREE_TYPE,
		};
		wxCheckBox* m_checkLib;
		wxCheckBox* m_checkUsr;
		FbFilterList* m_treeLang;
		FbFilterList* m_treeType;

	private:
		void OnNoButton( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif //__FBFILTERDLG_H__
