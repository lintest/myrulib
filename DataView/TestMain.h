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
#include <wx/dataview.h>

class DataViewFrame: public wxFrame
{
	public:
		DataViewFrame( wxWindow* parent, wxWindowID id = wxID_ANY);
		~DataViewFrame();

	protected:
		enum {
			idMenuQuit = 1000,
			idOpenList,
			idOpenTree,
			idMenuAbout,
			idDataView,
		};

		wxStatusBar* m_statusbar;
		wxDataViewCtrl* m_dataview;

    private:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnOpenList(wxCommandEvent& event);
        void OnOpenTree(wxCommandEvent& event);
		void OnActivated(wxDataViewEvent& event);
		void OnKeyUp(wxKeyEvent& event);
        DECLARE_EVENT_TABLE()
};

#endif // DATAVIEWMAIN_H
