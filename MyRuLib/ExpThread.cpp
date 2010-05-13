#include "ExpThread.h"
#include "FbParams.h"
#include "ZipReader.h"
#include <wx/tokenzr.h>

WX_DEFINE_OBJARRAY(ExportFileArray);

void ExportThread::WriteFileItem(ExportFileItem &item)
{
	ZipReader reader(item.id);
	if (!reader.IsOK()) {
		wxLogError( _("Export error") + (wxString)wxT(": ") + item.filename.GetFullPath() );
		return;
	}

	wxString filename = item.filename.GetFullPath();
	wxFileOutputStream out(filename);
	if (m_compress) {
		wxCSConv conv(wxT("cp866"));
		wxZipOutputStream zip(out, -1, conv);
		wxString entryName = item.filename.GetFullName();
		entryName = entryName.Left(entryName.Len()-4);
		zip.PutNextEntry(entryName);
		zip.Write(reader.GetZip());
	} else {
		out.Write(reader.GetZip());
	}
	out.Close();

	for (size_t i = 0; i < m_scripts.Count(); i++) {
		wxString script = m_scripts[i];
		if (script.IsEmpty()) continue;
		wxString command = GetCommand(script, filename);
		wxArrayString output;
		wxArrayString errors;
		wxLogInfo(_("Exec: ") + command);
		wxExecute(command, output, errors, wxEXEC_SYNC);
		for (size_t i = 0; i < output.Count(); i++)
			if (!output[i].IsEmpty()) wxLogInfo(output[i]);
		for (size_t i = 0; i < errors.Count(); i++)
			if (!errors[i].IsEmpty()) wxLogError(errors[i]);
	}
}

wxString ExportThread::GetCommand(const wxString &script, const wxString &filename)
{
	wxString result;
	bool param = false;
	for (size_t i=0; i < script.Length(); i++) {
		wxChar ch = script[i];
		if (param) {
			switch (ch) {
				case wxT('f'): {
					result += filename;
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

void *ExportThread::Entry()
{
	if (FbParams::GetValue(FB_SHELL_EXECUTE)) {
		wxStringTokenizer tkz(FbParams::GetText(FB_SHELL_COMMAND), wxT("\n"));
		while (tkz.HasMoreTokens()) m_scripts.Add(tkz.GetNextToken());
	}

	DoStart(m_filelist.Count(), wxEmptyString);

	for (size_t i=0; i<m_filelist.Count(); i++) {
		DoStep(m_filelist[i].filename.GetFullName());
		WriteFileItem(m_filelist[i]);
	}

	DoFinish();

	return NULL;
}

bool ExportThread::Execute()
{
	if ( Create() != wxTHREAD_NO_ERROR ) {
		wxLogError(_("Can't create export thread!"));
		return false;
	}
	Run();
	return true;
}
