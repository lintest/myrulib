#include "ExpThread.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "FbManager.h"
#include "FbParams.h"
#include "ZipReader.h"

WX_DEFINE_OBJARRAY(ExportFileArray);

void ExportThread::WriteFileItem(ExportFileItem &item)
{
    ZipReader reader(item.id);
    if (!reader.IsOK()) {
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_ERROR );
		event.SetString(reader.GetErrorText());
		wxPostEvent( m_frame, event );
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
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetInt(m_filelist.Count());
		event.SetString(m_info);
		wxPostEvent( m_frame, event );
	}

	int progress = 0;
	for (size_t i=0; i<m_filelist.Count(); i++)
	{
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
        event.SetString(m_filelist[i].filename.GetFullName());
        event.SetInt(progress);
        wxPostEvent( m_frame, event );
        progress++;

	    WriteFileItem(m_filelist[i]);
	}

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
		wxPostEvent( m_frame, event );
	}

	return NULL;
}
