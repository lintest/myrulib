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
		typedef struct {
			wxString title;
			wxString genre;
			wxString lang;
			wxString type;
			wxString dscr;
			wxString file;
			wxString path;
			wxString md5s;
			int date;
			int arch;
			wxLongLong size;
		} BookData;

		class TitlePanel: public wxScrolledWindow
		{
			public:
				TitlePanel(wxWindow* parent, int book, wxSQLite3Database &database, wxSQLite3ResultSet &result);
				void GetAuths(wxArrayInt &list, wxString &text);
				void SaveSeqn(wxSQLite3Database &database);
				void SaveGenr(wxSQLite3Database &database);
				void GetData(BookData & data);
			protected:
				void ArrangeControls(int height);
			private:
				int m_book;
				wxBoxSizer m_authors;
				wxBoxSizer m_series;
				wxBoxSizer m_genres;
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
				wxString GetValue() { return m_text.GetValue(); }
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
				int GetCode();
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
				virtual ~SeqnPanel();
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new SeqnPanel(parent, owner); }
				virtual void Empty()
					{}
			private:
				void StartThread();
				wxToolBar m_toolbar;
				FbComboBox m_text;
				wxTimer m_timer;
				FbThread * m_thread;
				wxTextCtrl m_numb;
				int m_code;
			private:
				void OnText( wxCommandEvent& event );
				void OnTextEnter( wxCommandEvent& event );
				void OnTimer( wxTimerEvent& event );
				void OnModel( FbArrayEvent& event );
				DECLARE_CLASS(SeqnPanel);
				DECLARE_EVENT_TABLE()
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

		class SearchThread : public FbThread
		{
			public:
				SearchThread(wxEvtHandler * frame, wxWindowID id,  const wxString & table, const wxString & text)
					: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_table(table), m_text(text), m_id(id) {}
			protected:
				virtual void * Entry();
			private:
				wxEvtHandler * m_frame;
				const wxString m_table;
				const wxString m_text;
				int m_id;
		};

	public:
		static bool Execute(int book);
		FbTitleDlg(wxWindow* parent, int book, wxSQLite3Database &database, wxSQLite3ResultSet &result);
		~FbTitleDlg();
		void ArrangeControls(int height);

	private:
		void Save(int book, wxSQLite3Database &database, wxSQLite3ResultSet &result);
		void Init();

	private:
		wxNotebook * m_notebook;
		TitlePanel * m_title;
		DescrPanel * m_descr;
};

#endif // __FBTITLEDLG_H__
