#ifndef __EXPTHREAD_H__
#define __EXPTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>
#include <wx/process.h>
#include "BaseThread.h"

class ExportFileItem
{
	public:
		ExportFileItem(const wxFileName &n, const int i): filename(n), id(i) {};
	public:
		wxFileName filename;
		int id;
};

WX_DECLARE_OBJARRAY(ExportFileItem, ExportFileArray);

class FbExportDlg : public wxFrame
{
	private:
		class ExportThread: public wxThread
		{
			public:
				ExportThread(int format, const ExportFileItem &item)
					: wxThread(wxTHREAD_JOINABLE), m_format(format), m_id(item.id), m_filename(item.filename.GetFullPath()) {}
				virtual void * Entry();
			private:
				int m_format;
				int m_id;
				wxString m_filename;
		};
		class ExportProcess: public wxProcess
		{
			public:
				ExportProcess(FbExportDlg * parent, const wxString& cmd)
					: m_parent(parent), m_cmd(cmd), m_running(true) {}
				virtual void OnTerminate(int pid, int status);
				virtual bool HasInput();
				bool IsRunning() { return m_running; }
			protected:
				FbExportDlg * m_parent;
				wxString m_cmd;
				bool m_running;
		};
	public:
		FbExportDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~FbExportDlg();
		void Execute();
		wxListBox & GetLog() { return m_text; }
		ExportFileArray m_filelist;
		int m_format;
	private:
		void Start();
		wxString GetScript(int format);
		wxString GetCommand(const wxString &script, const wxFileName &filename);
		void ExportFile(const ExportFileItem &item);
		void ExecScript(const wxString &script, const wxFileName &filename);
		wxArrayString m_scripts;
		size_t m_index;
		wxProcess * m_process;
	private:
		wxStaticText m_info;
		wxListBox m_text;
		wxGauge m_gauge;
	private:
		void OnProcessTerm(wxProcessEvent& event);
		void OnCancel(wxCommandEvent& event) {}
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbExportDlg);
};

class ExportThread : public BaseThread
{
	public:
		ExportThread(int format): BaseThread(), m_format(format) {};
		virtual void *Entry();
	public:
		ExportFileArray m_filelist;
		bool Execute();
	private:
		void WriteFileItem(ExportFileItem &item);
		wxString GetScript();
		wxString GetCommand(const wxString &script, const wxFileName &filename);
	private:
		int m_format;
		wxArrayString m_scripts;
};

#endif // __EXPTHREAD_H__
