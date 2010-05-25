#include "ExpThread.h"
#include "FbParams.h"
#include "ZipReader.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/dialog.h>

WX_DEFINE_OBJARRAY(ExportFileArray);

//-----------------------------------------------------------------------------
//  FbExportDlg::ExportThread
//-----------------------------------------------------------------------------

void * FbExportDlg::ExportThread::Entry()
{
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_SCRIPT_LOG, m_filename).Post(m_parent);

	ZipReader reader(m_id);
	if (!reader.IsOK()) {
		wxString msg = wxT("E> "); msg << COLON << m_filename;
		FbCommandEvent(fbEVT_BOOK_ACTION, ID_SCRIPT_LOG, msg).Post(m_parent);
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
//  FbExportDlg::ExportProcess
//-----------------------------------------------------------------------------

void FbExportDlg::ExportProcess::OnTerminate(int pid, int status)
{
    { while (HasInput()) ; }
    FbCommandEvent(fbEVT_BOOK_ACTION, ID_SCRIPT_RUN).Post(m_parent);
}

bool FbExportDlg::ExportProcess::HasInput()
{
    bool hasInput = false;

	#ifdef __WXMSW__
	wxCSConv conv = wxCSConv(wxFONTENCODING_CP1251);
	#else
	wxConvAuto conv = wxConvAuto();
	#endif // __WXMSW__
	const wxString &sep=wxT(" \t");

    wxString msg, info;

    if ( IsInputAvailable() )
    {
        msg = _T("i> ");
        wxTextInputStream tis(*GetInputStream(), sep, conv);
        info = tis.ReadLine();
    }

    if ( IsErrorAvailable() )
    {
        msg = _T("E> ");
        wxTextInputStream tis(*GetErrorStream(), sep, conv);
        info = tis.ReadLine();
    }

    if (!info.IsEmpty()) {
    	msg << info;
		FbCommandEvent(fbEVT_BOOK_ACTION, ID_SCRIPT_LOG, msg).Post(m_parent);
        hasInput = true;
    }

    return hasInput;
}

//-----------------------------------------------------------------------------
//  FbExportDlg
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbExportDlg, FbDialog )

BEGIN_EVENT_TABLE( FbExportDlg, FbDialog )
	EVT_BUTTON( wxID_CANCEL, FbExportDlg::OnCancel )
	EVT_COMMAND( ID_SCRIPT_LOG, fbEVT_BOOK_ACTION, FbExportDlg::OnScriptLog )
	EVT_COMMAND( ID_SCRIPT_RUN, fbEVT_BOOK_ACTION, FbExportDlg::OnScriptRun )
    EVT_IDLE( FbExportDlg::OnIdle )
END_EVENT_TABLE()

FbExportDlg::FbExportDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog(parent, id, title, pos, size, style), m_format(0), m_index(0), m_script(0), m_process(this)
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

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxCANCEL );
	bSizerBottom->Add( sdbSizerBtn, 0, wxALIGN_CENTER_VERTICAL, 5 );

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
	if (m_script >= m_scripts.Count()) {
		m_script = 0;
		++m_index;
	}
	size_t count = m_filelist.Count();
	if (m_index >= count) {
		m_gauge.SetValue(count);
		if (m_index == count) wxMessageBox(_("Export completed successfully"));
		Destroy();
		return;
	}

	ExportFileItem & item = m_filelist[m_index];
	m_info.SetLabel(item.filename.GetFullPath());
	m_gauge.SetValue(m_index);
	if (m_script == 0) {
		ExportThread thread(this, m_format, item);
		if (thread.Create() == wxTHREAD_NO_ERROR) thread.Run();
		thread.Wait();
	}
	ExecScript(m_scripts[m_script++], item.filename);
}

void FbExportDlg::ExecScript(const wxString &script, const wxFileName &filename)
{
	if (script.IsEmpty()) return ;
	wxString command = GetCommand(script, filename);
	LogMessage(command);
	wxExecute(command, wxEXEC_ASYNC, &m_process);
}

void FbExportDlg::OnIdle(wxIdleEvent& event)
{
	if (m_process.HasInput()) event.RequestMore();
}

void FbExportDlg::OnCancel(wxCommandEvent& event)
{
	m_index = m_filelist.Count() + 1;
	m_process.Kill(wxSIGTERM);
}

void FbExportDlg::LogMessage(const wxString &msg)
{
	m_text.SetFirstItem(m_text.Append(msg));
}

void FbExportDlg::OnScriptLog(wxCommandEvent& event)
{
	LogMessage(event.GetString());
}

void FbExportDlg::OnScriptRun(wxCommandEvent& event)
{
	Start();
}
