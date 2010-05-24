#ifndef __EXPTHREAD_H__
#define __EXPTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>
#include <wx/process.h>
#include "BaseThread.h"
#include "FbWindow.h"

class ExportFileItem
{
	public:
		ExportFileItem(const wxFileName &n, const int i): filename(n), id(i) {};
	public:
		wxFileName filename;
		int id;
};

WX_DECLARE_OBJARRAY(ExportFileItem, ExportFileArray);

class FbExportDlg : public FbDialog
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
				ExportProcess(FbExportDlg * parent, const wxString& cmd = wxEmptyString)
					: m_parent(parent), m_cmd(cmd) { Redirect(); }
				virtual void OnTerminate(int pid, int status);
				virtual bool HasInput();
			protected:
				FbExportDlg * m_parent;
				wxString m_cmd;
		};
	public:
		FbExportDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
		~FbExportDlg();
		void Execute();
		void LogMessage(const wxString &msg);
		ExportFileArray m_filelist;
		int m_format;
		void Start();
	private:
		wxString GetScript(int format);
		wxString GetCommand(const wxString &script, const wxFileName &filename);
		void ExportFile(const ExportFileItem &item);
		void ExecScript(const wxString &script, const wxFileName &filename);
		wxArrayString m_scripts;
		size_t m_index;
		ExportProcess m_process;
	private:
		wxStaticText m_info;
		wxListBox m_text;
		wxGauge m_gauge;
	private:
		void OnIdle(wxIdleEvent& event);
		void OnProcessTerm(wxProcessEvent& event);
		void OnCancel(wxCommandEvent& event);
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
