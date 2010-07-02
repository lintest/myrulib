#include "ExpThread.h"
#include "FbParams.h"
#include "ZipReader.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/dialog.h>
#include <wx/cmdline.h>
#include <wx/zstream.h>

WX_DEFINE_OBJARRAY(ExportFileArray);

//-----------------------------------------------------------------------------
//  FbExportDlg::JoinedThread
//-----------------------------------------------------------------------------

void FbExportDlg::JoinedThread::OnExit()
{
	FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_EXIT).Post(m_parent);
}

void FbExportDlg::JoinedThread::Execute()
{
	if (Create() == wxTHREAD_NO_ERROR) Run(); else OnExit();
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportLog
//-----------------------------------------------------------------------------

FbExportDlg::ExportLog::ExportLog(FbExportDlg * parent)
	: m_parent(parent), m_old(wxLog::SetActiveTarget(this))
{
}

FbExportDlg::ExportLog::~ExportLog()
{
	wxLog::SetActiveTarget(m_old);
}

void FbExportDlg::ExportLog::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	switch ( level ) {
		case wxLOG_Error: {
			wxString msg = wxT("E> "); msg << (wxString)szString;
			FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_ERROR, msg).Post(m_parent);
		} break;
		case wxLOG_Warning: {
			wxString msg = wxT("!> "); msg << (wxString)szString;
			FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_LOG, msg).Post(m_parent);
		} break;
		case wxLOG_Info: {
			wxString msg = (wxString)szString;
			FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_LOG, msg).Post(m_parent);
		} break;
		default: {
			wxString msg = wxT("i> "); msg << (wxString)szString;
			FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_LOG, msg).Post(m_parent);
		} break;
	}
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportThread
//-----------------------------------------------------------------------------

FbExportDlg::ExportThread::ExportThread(FbExportDlg * parent, int format, const ExportFileItem &item)
	: JoinedThread(parent), m_format(format), m_id(item.id), m_filename(item.filename.GetFullPath())
{
}

void * FbExportDlg::ExportThread::Entry()
{
	ZipReader reader(m_id);
	if (!reader.IsOk()) {
		wxLogError(m_filename);
		return NULL;
	}

	wxFileOutputStream out(m_filename);
	if (!out.IsOk()) {
		wxLogError(m_filename);
		return NULL;
	}

	switch (m_format) {
		case -1: {
			wxFileName entry = m_filename;
			wxCSConv conv(wxT("cp866"));
			wxZipOutputStream zip(out, 9, conv);
			zip.PutNextEntry(entry.GetName());
			zip.Write(reader.GetZip());
		} break;
		case -2: {
			wxZlibOutputStream zip(out, 9, wxZLIB_GZIP);
			zip.Write(reader.GetZip());
		} break;
		default: {
			out.Write(reader.GetZip());
		} break;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbExportDlg::GzipThread
//-----------------------------------------------------------------------------

FbExportDlg::GzipThread::GzipThread(FbExportDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbExportDlg::GzipThread::Entry()
{
	size_t first = 0;
	size_t count = m_filelist.Count();
	if (count == 0) return NULL;
	wxString filename = m_filelist[0];
	if (count == 1) filename << wxT(".gz"); else first++;

	wxFileOutputStream out(filename);
	if (!out.IsOk()) return NULL;

	wxZlibOutputStream zip(out, 9, wxZLIB_GZIP);
	if (!zip.IsOk()) return NULL;

	if (first < count) {
		wxFileInputStream in(m_filelist[first]);
		if (in.IsOk()) zip.Write(in);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ZipThread
//-----------------------------------------------------------------------------

FbExportDlg::ZipThread::ZipThread(FbExportDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbExportDlg::ZipThread::Entry()
{
	size_t first = 0;
	size_t count = m_filelist.Count();
	if (count == 0) return NULL;
	wxString filename = m_filelist[0];
	if (count == 1) filename << wxT(".zip"); else first++;

	wxFileOutputStream out(filename);
	if (!out.IsOk()) return NULL;

	wxCSConv conv(wxT("cp866"));
	wxZipOutputStream zip(out, 9, conv);
	if (!zip.IsOk()) return NULL;

	for (size_t i = first; i < count; i++) {
		wxFileInputStream in(m_filelist[i]);
		if (!in.IsOk()) continue;
		wxFileName filename = m_filelist[i];
		zip.PutNextEntry(filename.GetFullName());
		zip.Write(in);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportThread
//-----------------------------------------------------------------------------

FbExportDlg::DelThread::DelThread(FbExportDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbExportDlg::DelThread::Entry()
{
	size_t count = m_filelist.Count();
	for (size_t i = 0; i < count; i++) {
		wxString filename = m_filelist[i];
		bool ok = wxRemoveFile(filename);
		if (!ok) wxLogError(_("can't delete file %s"), filename.c_str());
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportProcess
//-----------------------------------------------------------------------------

void FbExportDlg::ExportProcess::OnTerminate(int pid, int status)
{
    FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_RUN).Post(m_parent);
}

bool FbExportDlg::ExportProcess::HasInput()
{
	bool hasInput = false;

    if (IsInputAvailable()) {
    	wxString info = ReadLine(GetInputStream());
		if (!info.IsEmpty()) wxLogMessage(info);
		hasInput = true;
    }

    if (IsErrorAvailable()) {
    	wxString info = ReadLine(GetErrorStream());
		if (!info.IsEmpty()) wxLogError(info);
		hasInput = true;
    }

    return hasInput;
}

wxString FbExportDlg::ExportProcess::ReadLine(wxInputStream * stream)
{
	if (stream == NULL) return wxEmptyString;

	#ifdef __WXMSW__
	wxChar * charset[] = {wxT("cp866"), wxT("cp1251")};
	wxCSConv conv = wxCSConv(charset[m_dos ? 0 : 1]);
	#else
	wxConvAuto conv = wxConvAuto();
	#endif // __WXMSW__
	const wxString &sep=wxT(" \t");

	wxTextInputStream tis(*stream, sep, conv);
	return tis.ReadLine();
}

//-----------------------------------------------------------------------------
//  FbExportDlg
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbExportDlg, FbDialog )

BEGIN_EVENT_TABLE( FbExportDlg, FbDialog )
	EVT_BUTTON( wxID_CANCEL, FbExportDlg::OnCancelBtn )
	EVT_BUTTON( wxID_CLOSE, FbExportDlg::OnCloseBtn )
	EVT_COMMAND( ID_SCRIPT_RUN, fbEVT_EXPORT_ACTION, FbExportDlg::OnScriptRun )
	EVT_COMMAND( ID_SCRIPT_LOG, fbEVT_EXPORT_ACTION, FbExportDlg::OnScriptLog )
	EVT_COMMAND( ID_SCRIPT_ERROR, fbEVT_EXPORT_ACTION, FbExportDlg::OnScriptError )
	EVT_COMMAND( ID_SCRIPT_EXIT, fbEVT_EXPORT_ACTION, FbExportDlg::OnScriptExit )
	EVT_CLOSE( FbExportDlg::OnCloseDlg )
    EVT_IDLE( FbExportDlg::OnIdle )
END_EVENT_TABLE()

FbExportDlg::FbExportDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog(parent, id, title, pos, size, style), m_format(0), m_index(0), m_script(0), m_process(this), m_log(this), m_closed(false), m_errors(0), m_thread(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_info.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_info.Wrap( -1 );
	bSizerMain->Add( &m_info, 0, wxALL|wxEXPAND, 5 );

	m_text.Create( this, wxID_ANY);
	bSizerMain->Add( &m_text, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
    wxFont font(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    if (font.Ok()) m_text.SetFont(font);

	wxBoxSizer* bSizerBottom = new wxBoxSizer( wxHORIZONTAL );

	m_gauge.Create( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizerBottom->Add( &m_gauge, 1, wxALL, 5 );

	m_button.Create( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerBottom->Add( &m_button, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	SetEscapeId(wxID_CANCEL);

	bSizerMain->Add( bSizerBottom, 0, wxEXPAND, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	m_timer.Start(100);
}

FbExportDlg::~FbExportDlg()
{
	wxSafeYield(this);
	if (m_thread) m_thread->Wait();
	wxDELETE(m_thread);
	wxSafeYield(this);
}

wxString FbExportDlg::GetCommand(const wxString &script, const wxFileName &filename)
{
	wxString result;
	bool param = false;
	for (size_t i=0; i < script.Length(); i++) {
		wxChar ch = script[i];
		if (param) {
			switch (ch) {
				case wxT('f'): {
					result += filename.GetFullPath();
				} break;
				case wxT('n'): {
					result += filename.GetName();
				} break;
				case wxT('p'): {
					result += filename.GetPath() + filename.GetPathSeparator() + filename.GetName();
				} break;
				case wxT('e'): {
					result += filename.GetExt();
				} break;
				case wxT('d'): {
					result += filename.GetPath();
				} break;
				default: {
					result += ch;
				} break;
			}
			param = false;
		} else {
			switch (ch) {
				case wxT('%'): {
					param = true;
				} break;
				default: {
					result += ch;
				} break;
			}
		}
	}
	return result;
}

wxString FbExportDlg::GetScript(int format)
{
	wxString sql = wxT("SELECT text FROM script WHERE id=?");
	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, format);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow())
		return result.GetString(0);
	else return wxEmptyString;
}

void FbExportDlg::Execute()
{
	if (m_format > 0) {
		wxStringTokenizer tkz(GetScript(m_format), wxT("\n"), wxTOKEN_STRTOK);
		while (tkz.HasMoreTokens()) m_scripts.Add(tkz.GetNextToken());
	}
	m_gauge.SetRange(m_filelist.Count());
	m_index = 0;
	m_script = 0;
	Show();
	Start();
}

void FbExportDlg::Start()
{
	if (m_closed) {
		Destroy();
		return;
	}

	size_t scriptCount = m_scripts.Count();
	if (m_script > scriptCount) {
		if (scriptCount) wxLogInfo(wxEmptyString);
		m_script = 0;
		m_index++;
	}

	if (m_index >= m_filelist.Count()) {
		Finish();
		return;
	}

	ExportFileItem & item = m_filelist[m_index];
	m_info.SetLabel(item.filename.GetFullPath());
	m_gauge.SetValue(m_index);

	if (m_script == 0)
		ExportFile(m_script++, item);
	else
		ExecScript(m_script++, item.filename);
}

void FbExportDlg::Finish()
{
	size_t count = m_filelist.Count();
	m_gauge.SetValue(count);
	m_button.SetId(wxID_CLOSE);
	m_button.SetLabel(_("Close"));
	SetEscapeId(wxID_CLOSE);

	wxString msg;
	if (m_index == count) {
		if (m_errors) msg.Printf(_("Export completed with %d errors"), m_errors);
		else msg = _("Export completed successfully");
	} else msg = _("Export aborted by user");
	m_info.SetLabel(msg);
}

void FbExportDlg::ExportFile(size_t index, const ExportFileItem &item)
{
	wxLogInfo(item.filename.GetFullPath());
	m_thread = new ExportThread(this, m_format, item);
	m_thread->Execute();
}

void FbExportDlg::ExecScript(size_t index, const wxFileName &filename)
{
	if (index == 0 || index > m_scripts.Count()) { Start(); return; }
	wxString script = m_scripts[index - 1];

	if (script.IsEmpty()) { Start(); return; }
	wxString command = GetCommand(script, filename);
	wxLogInfo(command);

	#ifdef __WXMSW__
	m_process.m_dos = false;
	#endif // __WXMSW__

	wxArrayString args = wxCmdLineParser::ConvertStringToArgs(command.c_str());
	if (args.Count() > 1) {
		m_thread = NULL;
		wxString cmd = args[0].Lower();
		if (cmd == wxT("gzip")) { m_thread = new GzipThread(this, args); }
		else if (cmd == wxT("zip")) { m_thread = new ZipThread(this, args); }
		else if (cmd == wxT("del")) { m_thread = new DelThread(this, args); }
		else if (cmd == wxT("rm"))  { m_thread = new DelThread(this, args); }
		else if (cmd == wxT("gz")) { m_thread = new GzipThread(this, args); }
		#ifdef __WXMSW__
		else if (cmd == wxT("cmd")) m_process.m_dos = true;
		#endif // __WXMSW__
		if (m_thread) { m_thread->Execute(); return; }
	}
	long pid = wxExecute(command, wxEXEC_ASYNC, &m_process);
	if (!pid) { wxLogError(command); Start(); }
}

void FbExportDlg::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    wxWakeUpIdle();
}

void FbExportDlg::OnIdle(wxIdleEvent& event)
{
	if (m_process.HasInput()) event.RequestMore();
}

void FbExportDlg::OnCancelBtn(wxCommandEvent& event)
{
	m_index = m_filelist.Count() + 1;
	m_process.Kill(wxSIGTERM);
}

void FbExportDlg::LogMessage(const wxString &msg)
{
	m_text.SetFirstItem(m_text.Append(msg));
}

void FbExportDlg::OnScriptRun(wxCommandEvent& event)
{
	{ while (m_process.HasInput()) ; }
	Start();
}

void FbExportDlg::OnScriptExit(wxCommandEvent& event)
{
	if (m_thread) m_thread->Wait();
	wxDELETE(m_thread);
	Start();
}

void FbExportDlg::OnScriptLog(wxCommandEvent& event)
{
	LogMessage(event.GetString());
}

void FbExportDlg::OnScriptError(wxCommandEvent& event)
{
	LogMessage(event.GetString());
	m_errors++;
}

void FbExportDlg::OnCloseBtn(wxCommandEvent& event)
{
	Destroy();
}

void FbExportDlg::OnCloseDlg( wxCloseEvent& event )
{
	if (m_index >= m_filelist.Count()) {
		Destroy();
	} else {
		m_closed = true;
		m_index = m_filelist.Count() + 1;
		m_process.Kill(wxSIGTERM);
	}
}

