#ifndef __FBFILTERDLG_H__
#define __FBFILTERDLG_H__

#include <wx/wx.h>
#include <wx/intl.h>

#include <wx/wx.h>
#include "FbWindow.h"
#include "controls/FbTreeView.h"
#include "FbFilterObj.h"
#include "FbDatabase.h"
#include "FbThread.h"
#include "FbParams.h"
#include "FbBookEvent.h"

class FbFilterDlg : public FbDialog
{
	public:
		enum {
			ID_CHECK_LIB = 1000,
			ID_CHECK_USR,
			ID_TREE_LANG,
			ID_TREE_TYPE,
		};
		static bool Execute(FbFilterObj & filter);
		FbFilterDlg(FbFilterObj & filter);
		virtual ~FbFilterDlg();

	private:
		wxCheckBox* m_checkLib;
		wxCheckBox* m_checkUsr;
		FbTreeViewCtrl * m_treeLang;
		FbTreeViewCtrl * m_treeType;

	private:
		void Assign(FbFilterObj & filter);

	private:
		FbFilterObj m_filter;
		FbThread * m_thread;

	private:
		void OnNoButton( wxCommandEvent& event );
		void OnTreeModel( FbModelEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif //__FBFILTERDLG_H__
