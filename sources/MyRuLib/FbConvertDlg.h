#ifndef __FBCONVERTDLG_H__
#define __FBCONVERTDLG_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/timer.h>
#include "FbWindow.h"
#include "FbConst.h"
#include "FbBookEvent.h"

class FbConvertItem
{
	public:
		FbConvertItem(int id, const wxFileName &name): m_id(id), m_filename(name) {};
		wxFileName GetAbsolute(const wxString & root) const;
		wxString GetRelative() const { return m_filename.GetFullPath(); }
		int GetBook() const { return m_id; }
	private:
		int m_id;
		wxFileName m_filename;
};

WX_DECLARE_OBJARRAY(FbConvertItem, FbConvertArray);

class FbConvertDlg : public FbDialog
{
	private:
		class ExportLog: public wxLog
		{
			public:
				ExportLog(FbConvertDlg * parent);
				virtual ~ExportLog();
			protected:
				void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
				void DoLogString(const wxChar *szString, time_t t) {}
			private:
				FbConvertDlg * m_parent;
				wxLog * m_old;
				DECLARE_NO_COPY_CLASS(ExportLog)
		};
		class JoinedThread: public FbThread
		{
			public:
				JoinedThread(FbConvertDlg * parent)
					: FbThread(wxTHREAD_JOINABLE), m_parent(parent) {}
				void Execute();
			protected:
				virtual void OnExit();
				FbConvertDlg * m_parent;
		};
		class ExportThread: public JoinedThread
		{
			public:
				ExportThread(FbConvertDlg * parent, int format, int book, const wxFileName &filename);
			protected:
				virtual void * Entry();
			private:
				int m_format;
				int m_id;
				wxFileName m_filename;
		};
		class GzipThread: public JoinedThread
		{
			public:
				GzipThread(FbConvertDlg * parent, const wxArrayString &args);
			protected:
				virtual void * Entry();
			private:
				wxArrayString m_filelist;
		};
		class BzipThread: public JoinedThread
		{
			public:
				BzipThread(FbConvertDlg * parent, const wxArrayString &args);
			protected:
				virtual void * Entry();
			private:
				wxArrayString m_filelist;
		};
		class ZipThread: public JoinedThread
		{
			public:
				ZipThread(FbConvertDlg * parent, const wxArrayString &args);
			protected:
				virtual void * Entry();
			private:
				wxArrayString m_filelist;
		};
		class DelThread: public JoinedThread
		{
			public:
				DelThread(FbConvertDlg * parent, const wxArrayString &args);
			protected:
				virtual void * Entry();
			private:
				wxArrayString m_filelist;
		};
		class ExportProcess: public wxProcess
		{
			public:
				ExportProcess(FbConvertDlg * parent): m_parent(parent) { Redirect(); }
				virtual void OnTerminate(int pid, int status);
				virtual bool HasInput();
				#ifdef __WXMSW__
				bool m_dos;
				#endif // __WXMSW__
			protected:
				wxString ReadLine(wxInputStream * stream);
				FbConvertDlg * m_parent;
		};
	public:
		FbConvertDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
		~FbConvertDlg();
		void Execute();
		void LogMessage(const wxString &msg);
		FbConvertArray m_filelist;
		int m_format;
		wxString m_root;
	private:
		void Start();
		void Finish();
		wxString GetScript(int format);
		wxString GetCommand(const wxString &script, const wxFileName &filename);
		void ExportFile(size_t index, const FbConvertItem &item);
		void ExecScript(size_t index, const FbConvertItem &item);
		wxArrayString m_scripts;
		size_t m_index;
		size_t m_script;
		ExportProcess m_process;
		ExportLog m_log;
		bool m_closed;
		int m_errors;
		JoinedThread * m_thread;
	private:
		wxStaticText m_info;
		wxListBox m_text;
		wxGauge m_gauge;
		wxButton m_button;
		wxTimer m_timer;
	private:
		void OnTimer(wxTimerEvent& event);
		void OnIdle(wxIdleEvent& event);
		void OnProcessTerm(wxProcessEvent& event);
		void OnCancelBtn(wxCommandEvent& event);
		void OnCloseBtn(wxCommandEvent& event);
		void OnScriptRun(wxCommandEvent& event);
		void OnScriptLog(wxCommandEvent& event);
		void OnScriptError(wxCommandEvent& event);
		void OnScriptExit(wxCommandEvent& event);
		void OnCloseDlg(wxCloseEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbConvertDlg);
};

#endif // __FBCONVERTDLG_H__
