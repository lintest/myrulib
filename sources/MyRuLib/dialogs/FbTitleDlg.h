#ifndef __FBTITLEDLG_H__
#define __FBTITLEDLG_H__

#include <wx/wx.h>
#include <wx/combo.h>
#include <wx/datectrl.h>
#include <wx/wxsqlite3.h>
#include "FbConst.h"
#include "FbBookEvent.h"
#include "FbThread.h"
#include "FbWindow.h"
#include "controls/FbComboBox.h"

class FbListModel;

class FbTitleDlg : public FbDialog
{
	private:
		class TitlePanel: public wxScrolledWindow
		{
			public:
				TitlePanel( wxWindow* parent, int book, wxSQLite3Database &database, wxSQLite3ResultSet &result );
			protected:
				void ArrangeControls(int height);
			private:
				wxBoxSizer * m_authors;
				wxBoxSizer * m_series;
				wxBoxSizer * m_genres;
				wxTextCtrl m_title;
				wxTextCtrl m_lang;
				wxTextCtrl m_type;
				wxDatePickerCtrl m_date;
			protected:
				void OnToolAdd( wxCommandEvent& event );
				void OnToolDel( wxCommandEvent& event );
				DECLARE_EVENT_TABLE()
				DECLARE_CLASS(TitlePanel);
		};

		class DescrPanel: public wxPanel
		{
			public:
				DescrPanel( wxWindow* parent, int book, wxSQLite3ResultSet &result );
			private:
				wxTextCtrl m_text;
			protected:
				DECLARE_CLASS(DescrPanel);
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

		class AuthPanel: public SubPanel
		{
			public:
				AuthPanel( wxWindow* parent, wxBoxSizer * owner, int code = 0, const wxString & text = wxEmptyString );
				virtual ~AuthPanel();
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new AuthPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				void StartThread();
				wxToolBar m_toolbar;
				FbComboBox m_text;
				wxTimer m_timer;
				FbThread * m_thread;
				int m_code;
			private:
				void OnText( wxCommandEvent& event );
				void OnTextEnter( wxCommandEvent& event );
				void OnTimer( wxTimerEvent& event );
				void OnModel( FbArrayEvent& event );
				DECLARE_CLASS(AuthPanel)
				DECLARE_EVENT_TABLE()
		};

		class SeqnPanel: public SubPanel
		{
			public:
				SeqnPanel( wxWindow* parent, wxBoxSizer * owner, int code = 0, const wxString & text = wxEmptyString, int numb = 0);
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new SeqnPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				wxToolBar m_toolbar;
				FbCustomCombo m_text;
				wxTextCtrl m_numb;
				DECLARE_CLASS(SeqnPanel);
		};

		class GenrPanel: public SubPanel
		{
			public:
				GenrPanel( wxWindow* parent, wxBoxSizer * owner, const wxString & code = wxEmptyString, const wxString & text = wxEmptyString);
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new GenrPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				wxToolBar m_toolbar;
				FbComboBox m_text;
				wxTextCtrl m_numb;
				DECLARE_CLASS(GenrPanel);
		};

		class AuthThread : public FbThread
		{
			public:
				AuthThread(wxEvtHandler * frame, const wxString & name)
					: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_name(name) {}
			protected:
				virtual void * Entry();
			private:
				wxEvtHandler * m_frame;
				const wxString m_name;
		};

	public:
		static bool Execute(int book);
		FbTitleDlg( wxWindow* parent, int book );
		~FbTitleDlg();
		void ArrangeControls(int height);

	protected:
		wxNotebook * m_notebook;
		TitlePanel * m_title;
		DescrPanel * m_descr;
		void Init();
};

#endif // __FBTITLEDLG_H__
