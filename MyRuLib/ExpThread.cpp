#include "ExpThread.h"
#include "ZipReader.h"

WX_DEFINE_OBJARRAY(ExportFileArray);

void ExportThread::WriteFileItem(ExportFileItem &item)
{
	ZipReader reader(item.id);
	if (!reader.IsOK()) {
		wxLogError( _("Export error") + (wxString)wxT(": ") + item.filename.GetFullPath() );
		return;
	}

	wxFileOutputStream out(item.filename.GetFullPath());

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
}

void *ExportThread::Entry()
{
//	wxCriticalSectionLocker enter(sm_queue);

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
