#ifndef __FBBOOKPANEL_H__
#define __FBBOOKPANEL_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "controls/FbTreeView.h"
#include "FbBookData.h"
#include "FbBookTypes.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbMasterInfo.h"
#include "FbPreviewWindow.h"
#include "FbFilterObj.h"

class FbMasterData;

class FbMasterThread;

class FbBookPanel: public wxSplitterWindow
{
	public:
		FbBookPanel(wxWindow *parent, const wxSize& size, wxWindowID id);
		virtual ~FbBookPanel();
		void Reset(const FbMasterInfo &master, const FbFilterObj &filter);
		void UpdateMaster(FbMasterEvent & event);
		void UpdateInfo(int id);
		void SetOrderID(int id);
		int GetOrderID();
		bool IsOrderDesc();
		void RevertOrder();
		void Localize();
		wxString GetSelected();
		const FbMasterInfo & GetInfo() const { return m_master; };
		void DoPopupMenu(wxWindowID id);
	public:
		FbTreeViewCtrl * m_BookList;
		void EmptyBooks(const int selected  = 0);
		void AppendBook(BookTreeItemData & data, const wxString & authors = wxEmptyString);
		void AppendAuthor(int id, const wxString title, wxTreeItemData * data = NULL);
		void AppendSequence(int id, const wxString title, wxTreeItemData * data = NULL);
		void SetListMode(FbListMode mode);
		FbListMode GetListMode() { return m_listmode; }
		FbViewMode GetViewMode();
		void UpdateFonts(bool refresh = true);
		void CreateColumns(const wxArrayInt &columns);
	private:
		size_t GetSelected(wxString &selections);
		size_t GetSelected(wxArrayInt &items);
		void ResetPreview();
	private:
		FbPreviewWindow * m_BookInfo;
		void SetViewMode(int mode);
		void DoFolderAdd(const int folder);
		static void DoDeleteDownload(const wxString &sel, const int folder);
		static void DoCreateDownload(const wxString &sel, int count = 1);
		int GetRatingColumn();
		void ShowContextMenu(const wxPoint& pos);
		wxString m_AuthorName;
		FbListMode m_listmode;
		wxTreeItemId m_AuthorItem;
		wxTreeItemId m_SequenceItem;
		int m_selected;
		FbMasterInfo m_master;
		FbMasterThread * m_thread;
		wxWindowID m_owner;
	private:
		void OnAuthorInfo(wxCommandEvent& event);
		void OnBooksListViewSelected(wxTreeEvent & event);
		void OnBooksListActivated(wxTreeEvent & event);
		void OnDeleteDownload(wxCommandEvent & event);
		void OnDownloadBook(wxCommandEvent & event);
		void OnEditComments(wxCommandEvent & event);
		void OnEditBook(wxCommandEvent & event);
		void OnFavoritesAdd(wxCommandEvent & event);
		void OnOpenBook(wxCommandEvent & event);
		void OnChangeView(wxCommandEvent & event);
		void OnBookPage(wxCommandEvent & event);
		void OnSystemDownload(wxCommandEvent & event);
		void OnSubmenu(wxCommandEvent& event);
		void OnContextMenu(wxTreeEvent& event);
		void OnChangeRating(wxCommandEvent& event);
		void OnDeleteBooks(wxCommandEvent& event);
		void OnModifyBooks(wxCommandEvent& event);
		void OnLinkClicked(wxHtmlLinkEvent& event);
		void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
		void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
		void OnListModel( FbArrayEvent& event );
		void OnListArray( FbArrayEvent& event );
		void OnTreeModel( FbModelEvent& event );
		void OnCopy(wxCommandEvent& event);
		void OnSelectAll(wxCommandEvent& event);
		void OnUnselectAll(wxCommandEvent& event);
		DECLARE_CLASS(FbBookPanel)
		DECLARE_EVENT_TABLE()
};


#endif // __FBBOOKPANEL_H__
