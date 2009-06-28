#include <wx/zipstrm.h>
#include "FbThread.h"
#include "FbParams.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "Sequences.h"
#include "MyRuLibMain.h"
#include "BookInfo.h"

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;
extern wxString strParsingInfo;

FbThread::FbThread(wxEvtHandler *frame, const wxString &filename)
        : wxThread(), m_filename(filename), m_frame(frame)
{

}

void FbThread::OnExit()
{
}

int FbThread::AddArchive()
{
    wxFileName file_name(m_filename);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Archives archives(wxGetApp().GetDatabase());
	ArchivesRow * row = archives.New();
	row->id = BookInfo::NewId(DB_NEW_ARCHIVE);
	row->file_name = file_name.GetFullName();
	row->file_path = file_name.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	row->min_id_book = 0;
	row->max_id_book = 0;
	row->file_count = 0;
	row->file_size = 0;
	row->Save();
	return row->id;
}

bool FbThread::ParseXml(wxInputStream& stream, const wxString &name, const wxFileOffset size, int id_archive)
{
	BookInfo info(stream);

	long id_book = 0;
	{
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		wxFileName filename (name);
		wxString shortname = filename.GetName();
		if (shortname.ToLong(&id_book)) {
			wxString query = wxString::Format(wxT("DELETE FROM books WHERE id=%d"), id_book);
			wxGetApp().GetDatabase()->RunQuery(query);
		} else {
			id_book = - BookInfo::NewId(DB_NEW_BOOK);
		}
	}

	for (size_t i = 0; i<info.authors.Count(); i++) {
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		Books books(wxGetApp().GetDatabase());
		Bookseq bookseq(wxGetApp().GetDatabase());

		BooksRow * row = books.New();
		row->id = id_book;
		row->id_author = info.authors[i];
		row->title = info.title;
		row->annotation = info.annotation;
		row->genres = info.genres;
		row->file_size = size;
		row->file_name = name;
		row->id_archive = id_archive;
		row->Save();

		for (size_t j = 0; j<info.sequences.Count(); j++) {
			BookseqRow * seqRow = bookseq.New();
			seqRow->id_book = id_book;
			seqRow->id_seq = info.sequences[j].seq;
			seqRow->id_author = info.authors[i];
			seqRow->number = info.sequences[j].num;
			seqRow->Save();
		}
	}

	return true;
}

void *FbThread::Entry()
{
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	int id_archive = AddArchive();

	{
		wxFileName filename = m_filename;
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetInt(zip.GetTotalEntries());
		event.SetString(strParsingInfo + filename.GetFullName());
		wxPostEvent( m_frame, event );
	}

	int progress = 0;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetName(wxPATH_UNIX);
			if (filename.Right(4).Lower() == wxT(".fb2")) {
				wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
				event.SetString(filename);
				event.SetInt(progress++);
				wxPostEvent( m_frame, event );

				zip.OpenEntry(*entry);
                ParseXml(zip, filename, entry->GetSize(), id_archive);
			}
		}
		delete entry;
	}

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
		wxPostEvent( m_frame, event );
	}

	return NULL;
}

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
		event.SetString(strParsingInfo + filename.GetFullName());
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

void RegThread::OnExit()
{
}


