#ifndef __FBTITLEDLG_H__
#define __FBTITLEDLG_H__

#include <wx/wx.h>
#include <wx/combo.h>
#include <wx/checkbox.h>
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
	protected:
		typedef struct {
			wxString title;
			wxString genr;
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

		class AuthSizer : public wxBoxSizer
		{
			public:
				AuthSizer(wxWindow* parent, wxSQLite3Database &database, const wxString &ids);
				void Get(wxArrayInt &list, wxString &text);
		};

		class SeqnSizer : public wxBoxSizer
		{
			public:
				SeqnSizer(wxWindow* parent, wxSQLite3Database &database, const wxString &sql);
		};

		class GenrSizer : public wxBoxSizer
		{
			public:
				GenrSizer(wxWindow* parent, wxSQLite3Database &database, const wxString &sql);
		};

		class TitlePanel: public wxScrolledWindow
		{
			public:
				TitlePanel( wxWindow* parent);
				void GetAuths(wxArrayInt &list, wxString &text);
				void SaveSeqn(int book, wxSQLite3Database &database);
				void SaveGenr(int book, wxSQLite3Database &database);
				void GetData(BookData & data);
				wxString GetGenr();
			protected:
				void ArrangeControls(int height);
			protected:
				AuthSizer * m_authors;
				SeqnSizer * m_series;
				GenrSizer * m_genres;
				wxTextCtrl m_title;
				wxTextCtrl m_lang;
				wxTextCtrl m_type;
				wxDatePickerCtrl m_date;
			private:
				void OnToolAdd( wxCommandEvent& event );
				void OnToolDel( wxCommandEvent& event );
				DECLARE_EVENT_TABLE()
		};

		class DscrPanel: public wxPanel
		{
			public:
				DscrPanel( wxWindow* parent, int book, wxSQLite3ResultSet &result );
				wxString GetValue() { return m_text.GetValue(); }
			private:
				wxTextCtrl m_text;
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
		};

		class AuthPanel: public SubPanel
		{
			public:
				AuthPanel( wxWindow* parent, wxBoxSizer * owner, int code = 0, const wxString & text = wxEmptyString );
				virtual ~AuthPanel();
				virtual SubPanel * New( wxWindow* parent, wxBoxSizer * owner )
					{ return new AuthPanel(parent, owner); }
				virtual void Empty();
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
				virtual void Empty();
				int GetCode();
				int GetNumb();
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
				virtual void Empty();
				wxString GetCode();
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
		FbTitleDlg(const wxString &title);
		void ArrangeControls(int height);

};

class FbSingleTitleDlg : public FbTitleDlg
{
private:
	class MainPanel: public TitlePanel
	{
	public:
		MainPanel(wxWindow* parent, int book, wxSQLite3Database &database, wxSQLite3ResultSet &result);
	};

public:
	static bool Execute(int book);
	FbSingleTitleDlg(int book, wxSQLite3Database &database, wxSQLite3ResultSet &result);

private:
	void Save(int book, wxSQLite3Database &database, wxSQLite3ResultSet &result);

private:
	wxNotebook * m_notebook;
	MainPanel * m_title;
	DscrPanel * m_descr;
};

class FbGroupTitleDlg : public FbTitleDlg
{
protected:
	class MainPanel: public TitlePanel
	{
	public:
		MainPanel(wxWindow* parent, const wxArrayInt &items, const wxString &codes, wxSQLite3Database &database);
	private:
		wxCheckBox * cb_auth;
		wxCheckBox * cb_seqn;
		wxCheckBox * cb_genr;
	};

public:
	static bool Execute(const wxArrayInt &items);
	FbGroupTitleDlg(const wxArrayInt &items, const wxString &codes, wxSQLite3Database &database);
	void Save(const wxArrayInt &items, const wxString &codes, wxSQLite3Database &database);

private:
	static wxString GetCodes(const wxArrayInt &items);

private:
	MainPanel * m_title;

};

#endif // __FBTITLEDLG_H__
