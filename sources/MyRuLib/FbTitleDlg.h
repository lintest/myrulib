#ifndef __FBTITLEDLG_H__
#define __FBTITLEDLG_H__

#include <wx/wx.h>
#include <wx/combo.h>
#include "FbWindow.h"

class FbTitleDlg : public FbDialog
{
	private:
		class TitlePanel: public wxScrolledWindow
		{
			public:
				TitlePanel( wxWindow* parent );
			protected:
				void ArrangeControls();
			private:
				wxBoxSizer * m_authors;
				wxBoxSizer * m_series;
				wxBoxSizer * m_genres;
				wxTextCtrl m_title;
			protected:
				void OnToolAdd( wxCommandEvent& event );
				void OnToolDel( wxCommandEvent& event );
				DECLARE_EVENT_TABLE()
				DECLARE_CLASS(TitlePanel);
		};

		class AuthSubPanel: public wxPanel
		{
			public:
				AuthSubPanel( wxWindow* parent );
			private:
				wxToolBar m_toolbar;
				wxComboCtrl m_text;
				DECLARE_CLASS(AuthSubPanel);
		};

		class SeqnSubPanel: public wxPanel
		{
			public:
				SeqnSubPanel( wxWindow* parent );
			private:
				wxToolBar m_toolbar;
				wxComboCtrl m_text;
				wxTextCtrl m_numb;
				DECLARE_CLASS(SeqnSubPanel);
		};

	public:
		static bool Execute(int book);
		FbTitleDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER | wxTAB_TRAVERSAL );
		~FbTitleDlg();
		void ArrangeControls();

	protected:
		wxNotebook * m_notebook;
		void Init();
};

#endif // __FBTITLEDLG_H__
