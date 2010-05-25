#include "ExpThread.h"
#include "FbParams.h"
#include "ZipReader.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/dialog.h>
#include <wx/cmdline.h>

WX_DEFINE_OBJARRAY(ExportFileArray);

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportThread
//-----------------------------------------------------------------------------

void * FbExportDlg::ExportThread::Entry()
{
	ZipReader reader(m_id);
	if (!reader.IsOK()) {
		FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_ERROR, m_filename).Post(m_parent);
		return NULL;
	}

	wxFileOutputStream out(m_filename);
	if (m_format == -1) {
		wxCSConv conv(wxFONTENCODING_CP866);
		wxZipOutputStream zip(out, -1, conv);
		wxString entryName = m_filename.Left(m_filename.Len()-4);
		zip.PutNextEntry(entryName);
		zip.Write(reader.GetZip());
	} else {
		out.Write(reader.GetZip());
	}
	out.Close();

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ZipThread
//-----------------------------------------------------------------------------

FbExportDlg::ZipThread::ZipThread(FbExportDlg * parent, const wxArrayString &args)
	: wxThread(wxTHREAD_JOINABLE)
{
	size_t count = args.Count();
	if (count > 1) m_filename = args[1];
	for (size_t i = 2; i < count; i++) m_filelist.Add(args[i]);
}

void * FbExportDlg::ZipThread::Entry()
{
	wxFileOutputStream out(m_filename);
	wxCSConv conv(wxFONTENCODING_CP866);
	wxZipOutputStream zip(out, -1, conv);

	size_t count = m_filelist.Count();
	for (size_t i = 0; i < count; i++) {
		wxFileInputStream in(m_filelist[i]);
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
	: wxThread(wxTHREAD_JOINABLE)
{
	size_t count = args.Count();
	for (size_t i = 1; i < count; i++) m_filelist.Add(args[i]);
}

void * FbExportDlg::DelThread::Entry()
{
	size_t count = m_filelist.Count();
	for (size_t i = 0; i < count; i++) {
		wxRemoveFile(m_filelist[i]);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportProcess
//-----------------------------------------------------------------------------

void FbExportDlg::ExportProcess::OnTerminate(int pid, int status)
{
    { while (HasInput()) ; }
    FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_RUN).Post(m_parent);
}

bool FbExportDlg::ExportProcess::HasInput()
{
    bool hasInput = false;

	#ifdef __WXMSW__
	wxCSConv conv = wxCSConv(m_dos ? wxFONTENCODING_CP866 : wxFONTENCODING_CP1251);
	#else
	wxConvAuto conv = wxConvAuto();
	#endif // __WXMSW__
	const wxString &sep=wxT(" \t");

    if (IsInputAvailable()) {
        wxTextInputStream tis(*GetInputStream(), sep, conv);
        wxString info = tis.ReadLine();
		if (!info.IsEmpty()) FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_LOG, info).Post(m_parent);
        hasInput = true;
    }

    if (IsErrorAvailable()) {
        wxTextInputStream tis(*GetErrorStream(), sep, conv);
        wxString info = tis.ReadLine();
		if (!info.IsEmpty()) FbCommandEvent(fbEVT_EXPORT_ACTION, ID_SCRIPT_ERROR, info).Post(m_parent);
        hasInput = true;
    }

    return hasInput;
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
	EVT_CLOSE( FbExportDlg::OnCloseDlg )
    EVT_IDLE( FbExportDlg::OnIdle )
END_EVENT_TABLE()

FbExportDlg::FbExportDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog(parent, id, title, pos, size, style), m_format(0), m_index(0), m_script(0), m_process(this), m_closed(false), m_errors(0)
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
}

FbExportDlg::~FbExportDlg()
{
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
	try {
		FbLocalDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, format);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return result.GetString(0);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return wxEmptyString;
}

void FbExportDlg::Execute()
{
	if (m_format > 0) {
		wxStringTokenizer tkz(GetScript(m_format), wxT("\n"));
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

	if (m_script >= m_scripts.Count()) {
		m_text.SetFirstItem(m_text.Append(wxEmptyString));
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
	if (m_script == 0) ExportFile(item);
	ExecScript(m_scripts[m_script++], item.filename);
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
	wxMessageBox(msg);
}

void FbExportDlg::ExportFile(const ExportFileItem &item)
{
	LogMessage(item.filename.GetFullPath());
	ExportThread thread(this, m_format, item);
	if (thread.Create() == wxTHREAD_NO_ERROR) thread.Run();
	thread.Wait();
}

void FbExportDlg::ZipFiles(const wxArrayString &args)
{
    if (args.Count() >= 3) {
		ZipThread thread(this, args);
		if (thread.Create() == wxTHREAD_NO_ERROR) thread.Run();
		thread.Wait();
    }
	Start();
}

void FbExportDlg::DelFiles(const wxArrayString &args)
{
    if (args.Count() >= 2) {
		DelThread thread(this, args);
		if (thread.Create() == wxTHREAD_NO_ERROR) thread.Run();
		thread.Wait();
    }
	Start();
}

void FbExportDlg::ExecScript(const wxString &script, const wxFileName &filename)
{
	if (script.IsEmpty()) return ;
	wxString command = GetCommand(script, filename);
	LogMessage(command);

	#ifdef __WXMSW__
	m_process.m_dos = false;
	#endif // __WXMSW__

	wxArrayString args = wxCmdLineParser::ConvertStringToArgs(command.c_str());
	if (args.Count()) {
		wxString cmd = args[0].Lower();
		#ifdef __WXMSW__
		if (cmd == wxT("cmd")) m_process.m_dos = true;
		#endif // __WXMSW__
		if (cmd == wxT("zip")) { ZipFiles(args); return; }
		if (cmd == wxT("del")) { DelFiles(args); return; }
	}
	wxExecute(command, wxEXEC_ASYNC, &m_process);
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
	Start();
}

void FbExportDlg::OnScriptLog(wxCommandEvent& event)
{
	wxString msg = wxT("i> ");
	msg << event.GetString();
	LogMessage(msg);
}

void FbExportDlg::OnScriptError(wxCommandEvent& event)
{
	wxString msg = wxT("E> ");
	msg << event.GetString();
	LogMessage(msg);
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
