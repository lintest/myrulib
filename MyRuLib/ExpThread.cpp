#include "ExpThread.h"
#include "MyRuLibApp.h"
#include "FbManager.h"
#include "FbParams.h"
#include "ZipReader.h"

WX_DEFINE_OBJARRAY(ExportFileArray);

void ExportThread::WriteFileItem(ExportFileItem &item)
{
    ZipReader reader(item.id);
    if (!reader.IsOK()) {
		DoError(reader.GetErrorText());
        return;
    }

    wxFileOutputStream out(item.filename.GetFullPath());

    if (m_compress) {
        wxZipOutputStream zip(out);
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
    wxCriticalSectionLocker enter(sm_queue);

    DoStart(m_filelist.Count(), wxEmptyString);

	for (size_t i=0; i<m_filelist.Count(); i++) {
	    DoStep(m_filelist[i].filename.GetFullName());
	    WriteFileItem(m_filelist[i]);
	}

	DoFinish();

	return NULL;
}
