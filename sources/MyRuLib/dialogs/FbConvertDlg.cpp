#include "FbConvertDlg.h"
#include "FbFileReader.h"
#include "FbParams.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/dialog.h>
#include <wx/cmdline.h>
#include <wx/zstream.h>
#include "bzipstream.h"

//-----------------------------------------------------------------------------
//  FbConvertDlg::FbConvertItem
//-----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbConvertArray);

wxFileName FbConvertItem::GetAbsolute(const wxString & root) const
{
	wxFileName filename = m_filename;
	filename.MakeAbsolute(root);
	return filename;
}

//-----------------------------------------------------------------------------
//  FbConvertDlg::JoinedThread
//-----------------------------------------------------------------------------

void FbConvertDlg::JoinedThread::OnExit()
{
	FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_EXIT).Post(m_parent);
}

void FbConvertDlg::JoinedThread::Execute()
{
	if (Create() == wxTHREAD_NO_ERROR) Run(); else OnExit();
}

//-----------------------------------------------------------------------------
//  FbConvertDlg::ExportLog
//-----------------------------------------------------------------------------

FbConvertDlg::ExportLog::ExportLog(FbConvertDlg * parent)
	: m_parent(parent), m_old(wxLog::SetActiveTarget(this))
{
}

FbConvertDlg::ExportLog::~ExportLog()
{
	wxLog::SetActiveTarget(m_old);
}

void FbConvertDlg::ExportLog::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
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
//  FbConvertDlg::ExportThread
//-----------------------------------------------------------------------------

FbConvertDlg::ExportThread::ExportThread(FbConvertDlg * parent, int format, int book, const wxFileName &filename)
	: JoinedThread(parent), m_format(format), m_id(book), m_filename(filename)
{
}

void * FbConvertDlg::ExportThread::Entry()
{
	FbFileReader reader(m_id);
	if (!reader.IsOk()) {
		reader.ShowError();
		return NULL;
	}

	wxString fullpath = m_filename.GetFullPath();
	switch (m_format) {
		case -1: fullpath << wxT(".zip"); break;
		case -2: fullpath << wxT(".gz"); break;
		case -3: fullpath << wxT(".bz2"); break;
	}

	m_filename.Mkdir(0777, wxPATH_MKDIR_FULL);
	wxFileOutputStream out(fullpath);
	if (!out.IsOk()) {
		wxLogError(fullpath);
		return NULL;
	}

	switch (m_format) {
		case -1: {
			wxCSConv conv(wxT("cp866"));
			wxZipOutputStream zip(out, 9, conv);
			zip.PutNextEntry(m_filename.GetFullName());
			zip.Write(reader.GetStream());
			zip.Close();
		} break;
		case -2: {
			wxZlibOutputStream zip(out, 9, wxZLIB_GZIP);
			zip.Write(reader.GetStream());
			zip.Close();
		} break;
		case -3: {
			wxBZipOutputStream zip(out, 9);
			zip.Write(reader.GetStream());
			zip.Close();
		} break;
		default: {
			out.Write(reader.GetStream());
			out.Close();
		} break;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbConvertDlg::GzipThread
//-----------------------------------------------------------------------------

FbConvertDlg::GzipThread::GzipThread(FbConvertDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbConvertDlg::GzipThread::Entry()
{
	size_t first = 0;
	size_t count = m_filelist.Count();
	if (count == 0) return NULL;

	wxString filename = m_filelist[0];
	if (count == 1) filename << wxT(".bz2"); else first++;

	wxFileInputStream in(m_filelist[first]);
	if (!in.IsOk()) return NULL;

	wxFileOutputStream out(filename);
	if (!out.IsOk()) return NULL;

	wxZlibOutputStream zip(out, 9, wxZLIB_GZIP);
	if (!zip.IsOk()) return NULL;

	zip.Write(in);
	zip.Close();

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbConvertDlg::BzipThread
//-----------------------------------------------------------------------------

FbConvertDlg::BzipThread::BzipThread(FbConvertDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbConvertDlg::BzipThread::Entry()
{
	size_t first = 0;
	size_t count = m_filelist.Count();
	if (count == 0) return NULL;

	wxString filename = m_filelist[0];
	if (count == 1) filename << wxT(".bz2"); else first++;

	wxFileInputStream in(m_filelist[first]);
	if (!in.IsOk()) return NULL;

	wxFileOutputStream out(filename);
	if (!out.IsOk()) return NULL;

	wxBZipOutputStream zip(out, 9);
	if (!zip.IsOk()) return NULL;

	zip.Write(in);
	zip.Close();

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbConvertDlg::ZipThread
//-----------------------------------------------------------------------------

FbConvertDlg::ZipThread::ZipThread(FbConvertDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbConvertDlg::ZipThread::Entry()
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
	zip.Close();

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbConvertDlg::ExportThread
//-----------------------------------------------------------------------------

FbConvertDlg::DelThread::DelThread(FbConvertDlg * parent, const wxArrayString &args)
	: JoinedThread(parent)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbConvertDlg::DelThread::Entry()
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
//  FbConvertDlg::ExportProcess
//-----------------------------------------------------------------------------

void FbConvertDlg::ExportProcess::OnTerminate(int pid, int status)
{
	FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_RUN).Post(m_parent);
}

bool FbConvertDlg::ExportProcess::HasInput()
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

wxString FbConvertDlg::ExportProcess::ReadLine(wxInputStream * stream)
{
	if (stream == NULL) return wxEmptyString;

	#ifdef __WXMSW__
	wxString charset = m_dos ? (wxString) wxT("cp866") : (wxString) wxT("cp1251");
	wxCSConv conv = wxCSConv(charset);
	#else
	wxConvAuto conv = wxConvAuto();
	#endif // __WXMSW__
	const wxString &sep=wxT(" \t");

	wxTextInputStream tis(*stream, sep, conv);
	return tis.ReadLine();
}

//-----------------------------------------------------------------------------
//  FbConvertDlg
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbConvertDlg, FbDialog )

BEGIN_EVENT_TABLE( FbConvertDlg, FbDialog )
	EVT_BUTTON( wxID_CANCEL, FbConvertDlg::OnCancelBtn )
	EVT_BUTTON( wxID_CLOSE, FbConvertDlg::OnCloseBtn )
	EVT_COMMAND( ID_SCRIPT_RUN, fbEVT_EXPORT_ACTION, FbConvertDlg::OnScriptRun )
	EVT_COMMAND( ID_SCRIPT_LOG, fbEVT_EXPORT_ACTION, FbConvertDlg::OnScriptLog )
	EVT_COMMAND( ID_SCRIPT_ERROR, fbEVT_EXPORT_ACTION, FbConvertDlg::OnScriptError )
	EVT_COMMAND( ID_SCRIPT_EXIT, fbEVT_EXPORT_ACTION, FbConvertDlg::OnScriptExit )
	EVT_CLOSE( FbConvertDlg::OnCloseDlg )
	EVT_IDLE( FbConvertDlg::OnIdle )
END_EVENT_TABLE()

FbConvertDlg::FbConvertDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog(parent, id, title, pos, size, style), m_format(0), m_index(0), m_script(0), m_process(this), m_log(this), m_closed(false), m_errors(0), m_thread(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_info.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_info.Wrap( -1 );
	bSizerMain->Add( &m_info, 0, wxALL|wxEXPAND, 5 );

	m_text.Create( this, wxID_ANY);
	bSizerMain->Add( &m_text, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	wxFont font(GetFont().GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
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

FbConvertDlg::~FbConvertDlg()
{
	wxSafeYield(this);
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
	wxSafeYield(this);
}

wxString FbConvertDlg::GetCommand(const wxString &script, const wxFileName &filename)
{
	wxString result;
	bool param = false;
	size_t length = script.Length();
	for (size_t i=0; i < length; i++) {
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

wxString FbConvertDlg::GetScript(int format)
{
	return FbLocalDatabase().Str(format, wxT("SELECT text FROM script WHERE id=?"));
}

void FbConvertDlg::Execute()
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

void FbConvertDlg::Start()
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

	FbConvertItem & item = m_filelist[m_index];
	m_info.SetLabel(item.GetRelative());
	m_gauge.SetValue(m_index);

	if (m_script == 0)
		ExportFile(m_script++, item);
	else
		ExecScript(m_script++, item);
}

void FbConvertDlg::Finish()
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

void FbConvertDlg::ExportFile(size_t index, const FbConvertItem &item)
{
	wxLogInfo(item.GetRelative());
	wxFileName filename = item.GetAbsolute(m_root);
	m_thread = new ExportThread(this, m_format, item.GetBook(), filename);
	m_thread->Execute();
}

void FbConvertDlg::ExecScript(size_t index, const FbConvertItem &item)
{
	wxFileName filename = item.GetAbsolute(m_root);

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
		else if (cmd == wxT("gz"))  { m_thread = new GzipThread(this, args); }
		else if (cmd == wxT("bz2")) { m_thread = new BzipThread(this, args); }
		#ifdef __WXMSW__
		else if (cmd == wxT("cmd")) m_process.m_dos = true;
		#endif // __WXMSW__
		if (m_thread) { m_thread->Execute(); return; }
	}
	long pid = wxExecute(command, wxEXEC_ASYNC, &m_process);
	if (!pid) { wxLogError(command); Start(); }
}

void FbConvertDlg::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	wxWakeUpIdle();
}

void FbConvertDlg::OnIdle(wxIdleEvent& event)
{
	if (m_process.HasInput()) event.RequestMore();
}

void FbConvertDlg::OnCancelBtn(wxCommandEvent& event)
{
	m_index = m_filelist.Count() + 1;
	m_process.Kill(wxSIGTERM);
}

void FbConvertDlg::LogMessage(const wxString &msg)
{
	m_text.SetFirstItem(m_text.Append(msg));
}

void FbConvertDlg::OnScriptRun(wxCommandEvent& event)
{
	{ while (m_process.HasInput()) ; }
	Start();
}

void FbConvertDlg::OnScriptExit(wxCommandEvent& event)
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
	Start();
}

void FbConvertDlg::OnScriptLog(wxCommandEvent& event)
{
	LogMessage(event.GetString());
}

void FbConvertDlg::OnScriptError(wxCommandEvent& event)
{
	LogMessage(event.GetString());
	m_errors++;
}

void FbConvertDlg::OnCloseBtn(wxCommandEvent& event)
{
	Destroy();
}

void FbConvertDlg::OnCloseDlg( wxCloseEvent& event )
{
	if (m_index >= m_filelist.Count()) {
		Destroy();
	} else {
		m_closed = true;
		m_index = m_filelist.Count() + 1;
		m_process.Kill(wxSIGTERM);
	}
}

