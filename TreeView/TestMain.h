/***************************************************************
 * Name:      DataViewMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef DATAVIEWMAIN_H
#define DATAVIEWMAIN_H

#include <wx/wx.h>
#include <wx/listbase.h>

class wxTreeEvent;

class FbAuthListInfo;

class FbTreeViewCtrl;

class DataViewFrame: public wxFrame
{
	public:
		DataViewFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("wxWidgets Application Template"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 481,466 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~DataViewFrame();
		static wxString sm_filename;
	protected:
		void CreateTreeModel();
		void CreateListModel();

	protected:
		enum
		{
			idMenuQuit = 1000,
			idOpenList,
			idOpenBook,
			idOpenTree,
			idMenuAbout,
			idSearchBtn,
			idCreateTree,
			idCreateList,
			ID_TYPE_LIST,
			ID_APPEND_TYPE,
			ID_MODIFY_TYPE,
			ID_DELETE_TYPE,
		};

		wxStatusBar * m_statusbar;
		FbTreeViewCtrl * m_dataview;

	private:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
		void OnOpenBook(wxCommandEvent& event);
		void OnOpenList(wxCommandEvent& event);
		void OnOpenTree(wxCommandEvent& event);
        void OnSearchBtn(wxCommandEvent& event);

		void OnCreateList(wxCommandEvent& event);
		void OnCreateTree(wxCommandEvent& event);

		void OnAppendType(wxCommandEvent& event);
		void OnModifyType(wxCommandEvent& event);
		void OnDeleteType(wxCommandEvent& event);
		void OnTypeActivated(wxTreeEvent& event);
		void OnColumnClick(wxListEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif // DATAVIEWMAIN_H
