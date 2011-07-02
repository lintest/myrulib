#ifndef __FBTITLEDLG_H__
#define __FBTITLEDLG_H__

#include <wx/wx.h>
#include <wx/combo.h>
#include <wx/wxsqlite3.h>
#include "FbWindow.h"
#include "controls/FbComboBox.h"

class FbListModel;

class FbTitleDlg : public FbDialog
{
	private:
		class TitlePanel: public wxScrolledWindow
		{
			public:
				TitlePanel( wxWindow* parent, int book );
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

		class SubPanel: public wxPanel
		{
			public:
				SubPanel( wxWindow* parent, wxBoxSizer * owner );
				wxBoxSizer * GetOwner()  { return m_owner; }
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner ) = 0;
				virtual void Empty() = 0;
			private:
				wxBoxSizer * m_owner;
				DECLARE_CLASS(SubPanel);
		};

		class AuthSubPanel: public SubPanel
		{
			public:
				AuthSubPanel( wxWindow* parent, wxBoxSizer * owner, int code = 0, const wxString & text = wxEmptyString );
				virtual ~AuthSubPanel();
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new AuthSubPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				FbListModel * CreateModel();
			private:
				wxToolBar m_toolbar;
				FbComboBox m_text;
			private:
				void OnChar( wxKeyEvent& event );
				void OnText( wxCommandEvent& event );
				DECLARE_CLASS(AuthSubPanel);
		};

		class SeqnSubPanel: public SubPanel
		{
			public:
				SeqnSubPanel( wxWindow* parent, wxBoxSizer * owner, int code = 0, const wxString & text = wxEmptyString, int numb = 0);
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new SeqnSubPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				wxToolBar m_toolbar;
				FbComboBox m_text;
				wxTextCtrl m_numb;
				DECLARE_CLASS(SeqnSubPanel);
		};

		class GenrSubPanel: public SubPanel
		{
			public:
				GenrSubPanel( wxWindow* parent, wxBoxSizer * owner, const wxString & code = wxEmptyString, const wxString & text = wxEmptyString);
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new GenrSubPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				wxToolBar m_toolbar;
				FbComboBox m_text;
				wxTextCtrl m_numb;
				DECLARE_CLASS(GenrSubPanel);
		};

	public:
		static bool Execute(int book);
		FbTitleDlg( wxWindow* parent, int book );
		~FbTitleDlg();
		void ArrangeControls();

	protected:
		wxNotebook * m_notebook;
		void Init();
};

#endif // __FBTITLEDLG_H__
