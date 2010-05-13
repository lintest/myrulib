#include "ExpThread.h"
#include "FbParams.h"
#include "ZipReader.h"

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

	if (!m_script.IsEmpty()) {
		wxArrayString output;
		wxArrayString errors;
		wxString command = m_script + wxT(" \"") + filename + wxT("\"");
		wxExecute(command, output, errors, wxEXEC_SYNC);
		for (size_t i = 0; i < output.Count(); i++)
			if (!output[i].IsEmpty()) wxLogWarning(output[i]);
		for (size_t i = 0; i < errors.Count(); i++)
			if (!errors[i].IsEmpty()) wxLogError(errors[i]);
	}
}

void *ExportThread::Entry()
{
//	wxCriticalSectionLocker enter(sm_queue);

	if (FbParams::GetValue(FB_SHELL_EXECUTE))
		m_script = FbParams::GetText(FB_SHELL_COMMAND);

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
