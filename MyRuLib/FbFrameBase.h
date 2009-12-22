#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbBookPanel.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbWindow.h"
#include "FbThread.h"
#include "FbMainMenu.h"

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
		virtual wxToolBar * CreateToolBar(long style, wxWindowID winid, const wxString& name);
		bool m_FilterFb2;
		bool m_FilterLib;
		bool m_FilterUsr;
		void UpdateInfo(int id);
		virtual void UpdateFonts(bool refresh = true);
		wxString GetOrderSQL() { return m_BooksPanel->GetOrderSQL(); };
		void UpdateStatus();
		virtual void ShowFullScreen(bool show);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist() = 0;
		virtual wxMenuBar * CreateMenuBar();
		virtual wxString GetStatus();
		int GetModeKey();
		int GetViewKey();
		void OnSubmenu(wxCommandEvent& event);
		void CreateBooksPanel(wxWindow * parent, long substyle);
		int GetColOrder(int col);
		wxString Naming(int count, const wxString &single, const wxString &genitive, const wxString &plural);
		int GetBookCount();
		bool IsFullScreen();
	protected:
		FbTreeListCtrl * m_MasterList;
		FbBookPanel * m_BooksPanel;
		wxToolBar * m_ToolBar;
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

	protected:
		class AggregateFunction : public wxSQLite3AggregateFunction
		{
			public:
				virtual void Aggregate(wxSQLite3FunctionContext& ctx);
				virtual void Finalize(wxSQLite3FunctionContext& ctx);
		};

		class BaseThread: public FbThread
		{
			public:
				BaseThread(FbFrameBase * frame, FbListMode mode)
					:m_frame(frame), m_mode(mode),
					m_FilterFb2(frame->m_FilterFb2),
					m_FilterLib(frame->m_FilterLib),
					m_FilterUsr(frame->m_FilterUsr),
					m_ListOrder(frame->GetOrderSQL())
				{};
			protected:
				virtual wxString GetSQL(const wxString & condition);
				virtual wxString GetOrder();
				virtual void CreateList(wxSQLite3ResultSet &result);
				virtual void CreateTree(wxSQLite3ResultSet &result);
				virtual void InitDatabase(FbCommonDatabase &database);
				void EmptyBooks();
				void FillBooks(wxSQLite3ResultSet &result);
			protected:
				static wxCriticalSection sm_queue;
				AggregateFunction m_aggregate;
				FbGenreFunction m_genre;
				wxWindow * m_frame;
				FbListMode m_mode;
				bool m_FilterFb2;
				bool m_FilterLib;
				bool m_FilterUsr;
				wxString m_ListOrder;
		};

	private:
		class MenuBar: public FbFrameMenu
		{
			public:
				MenuBar();
		};
};

#endif //__FBFRAMEBASE_H__

