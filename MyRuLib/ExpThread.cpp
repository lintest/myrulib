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
	ZipReader reader(m_id);
	if (!reader.IsOK()) {
		wxLogError( _("Export error") + (wxString)wxT(": ") + m_filename );
		return NULL;
	}
	wxFileOutputStream out(m_filename);
	if (m_format == -1) {
		wxCSConv conv(wxT("cp866"));
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
    wxProcess::OnTerminate(pid, status);
    m_running = false;
}

bool FbExportDlg::ExportProcess::HasInput()
{
    bool hasInput = false;

    if ( IsInputAvailable() )
    {
        wxTextInputStream tis(*GetInputStream());

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << _T(" (stdout): ") << tis.ReadLine();

        m_parent->GetLog().Append(msg);

        hasInput = true;
    }

    if ( IsErrorAvailable() )
    {
        wxTextInputStream tis(*GetErrorStream());

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << _T(" (stderr): ") << tis.ReadLine();

        m_parent->GetLog().Append(msg);

        hasInput = true;
    }

    return hasInput;
}

//-----------------------------------------------------------------------------
//  FbExportDlg
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS( FbExportDlg, wxFrame )

BEGIN_EVENT_TABLE( FbExportDlg, wxFrame )
	EVT_BUTTON( wxID_CANCEL, FbExportDlg::OnCancel )
    EVT_END_PROCESS(wxID_ANY, FbExportDlg::OnProcessTerm)
END_EVENT_TABLE()

FbExportDlg::FbExportDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxFrame(parent, id, title, pos, size, style), m_format(0), m_index(0), m_process(NULL)
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

//	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxCANCEL );
//	bSizerBottom->Add( sdbSizerBtn, 0, wxALIGN_CENTER_VERTICAL, 5 );

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
	Show();
	Start();
}

void FbExportDlg::Start()
{
	if (m_index >= m_filelist.Count()) return;

	ExportFileItem & item = m_filelist[m_index];
	m_gauge.SetValue(m_index);
	m_info.SetLabel(item.filename.GetFullName());
	ExportFile(item);
	m_index++;
}

void FbExportDlg::OnProcessTerm(wxProcessEvent& event)
{
	wxDELETE(m_process);
	Start();
}

void FbExportDlg::ExportFile(const ExportFileItem &item)
{
	ExportThread thread(m_format, item);
	if (thread.Create() == wxTHREAD_NO_ERROR) thread.Run();
	thread.Wait();

	for (size_t i = 0; i < m_scripts.Count(); i++)
		ExecScript(m_scripts[i], item.filename);
}

void FbExportDlg::ExecScript(const wxString &script, const wxFileName &filename)
{
	if (script.IsEmpty()) return ;
	wxString command = GetCommand(script, filename);
	wxLogInfo(_("Exec: ") + command);
/*
    wxArrayString output, errors;
    wxExecute(command, output, errors);
    for (size_t i = 0; i < output.Count(); i++) m_text.Append(output[i]);
    for (size_t i = 0; i < errors.Count(); i++) m_text.Append(errors[i]);
*/
	m_process = new ExportProcess(this, command);
	wxExecute(command, wxEXEC_ASYNC, m_process);
}
