#include "RegThread.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "FbManager.h"
#include "FbParams.h"
#include <wx/zipstrm.h>

extern wxString strParsingInfo;

RegThread::RegThread(wxEvtHandler *frame, const wxString &filename)
        : wxThread(), m_filename(filename), m_frame(frame)
{
}

int RegThread::AddArchive(int min, int max)
{
    wxFileName file_name(m_filename);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Archives archives(wxGetApp().GetDatabase());

    wxString name = file_name.GetFullName();
	ArchivesRow * row = archives.Where(wxString::Format(wxT("file_name='%s'"), name.c_str()));
	if (!row) {
        row = archives.New();
        row->id = BookInfo::NewId(DB_NEW_ARCHIVE);
	}

	row->file_name = file_name.GetFullName();
	row->file_path = file_name.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	row->min_id_book = min;
	row->max_id_book = max;
	row->file_count = 0;
	row->file_size = 0;
	row->Save();
	return row->id;
}

void *RegThread::Entry()
{
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	{
		wxFileName filename = m_filename;
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetInt(zip.GetTotalEntries());
		event.SetString(m_info + wxT(" ") + filename.GetFullName());
		wxPostEvent( m_frame, event );
	}

	int progress = 0;
	int min = 0;
    int max = 0;
    int step = 0;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetName(wxPATH_UNIX);
			if (filename.Right(4).Lower() == wxT(".fb2")) {
			    if (step == 1000) {
                    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
                    event.SetString(filename);
                    event.SetInt(progress);
                    wxPostEvent( m_frame, event );
                    step = 0;
			    }
			    progress++;
			    step++;

                wxString name = wxFileName(filename).GetName();
                long number = 0;
                if (name.ToLong(&number)) {
                    min = (min ? (number < min ? number : min) : number);
                    max = (number > max ? number : max);
                }
			}
		}
		delete entry;
	}

	AddArchive(min, max);

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
		wxPostEvent( m_frame, event );
	}

	return NULL;
}

