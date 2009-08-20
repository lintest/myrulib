#include <wx/dir.h>
#include <wx/thread.h>
#include "ZipReader.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "db/ZipBooks.h"
#include "db/ZipFiles.h"

extern wxString strBookNotFound;

class ZipCollection {
	public:
		ZipCollection();
		void Init(const wxString &dirname);
		virtual ~ZipCollection();
		wxString FindZip(const wxString &filename);
		void SetDir(const wxString &dirname);
		void AddZip(const wxString &filename);
	private:
		static wxCriticalSection sm_queue;
		wxString m_dirname;
		wxFileName m_tempfile;
		SqliteDatabaseLayer m_tempdata;
		int m_id;
};

static ZipCollection zips;

class ZipThread : public wxThread
{
public:
    virtual void *Entry();
};

void *ZipThread::Entry()
{
	zips.SetDir(FbParams::GetText(FB_LIBRARY_DIR));

	return NULL;
}

ZipReader::ZipReader(int id)
    :m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
    wxString zip_name, zip_path, file_name;
    int id_archive;

    {
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

        zip_path = FbParams::GetText(FB_LIBRARY_DIR);

        Books books(wxGetApp().GetDatabase());
        BooksRow * bookRow = books.Id(id);
        if (!bookRow) return;

        file_name = bookRow->file_name;
        id_archive = bookRow->id_archive;

        if (bookRow->id_archive) {
            ArchivesRow * archiveRow = bookRow->GetArchive();
            if (!archiveRow) return;
            wxFileName zip_file = archiveRow->file_name;
            zip_file.SetPath(archiveRow->file_path);
            zip_name = zip_file.GetFullPath();
        }
    }

    if (id_archive) {
        m_zipOk = wxFileName::FileExists(zip_name);
        if (m_zipOk) OpenZip(zip_name, file_name);
    } else if (wxFileName::FileExists(file_name)) {
        OpenFile(file_name);
    } else {
        zip_name = zips.FindZip(file_name);
        if (zip_name.IsEmpty()) return;
        m_zipOk = wxFileName::FileExists(zip_name);
        if (m_zipOk) OpenZip(zip_name, file_name);
    }
}

ZipReader::~ZipReader()
{
	wxDELETE(m_zip);
	wxDELETE(m_file);
}

void ZipReader::Init()
{
	ZipThread *thread = new ZipThread;

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }

    thread->Run();

    return;
}

void ZipReader::OpenZip(const wxString &zipname, const wxString &filename)
{
    m_file = new wxFFileInputStream(zipname);
    m_zip = new wxZipInputStream(*m_file);
    m_result = m_zip;

    m_zipOk  = m_file->IsOk();
    m_fileOk = FindEntry(filename);

    wxString zipText = (zipname.IsNull() ? zipname : wxT(" ") + zipname );
    wxString fileText = (filename.IsNull() ? filename : wxT(" ") + filename );
    m_info = wxString::Format(strBookNotFound, zipText.c_str(), fileText.c_str());

}

void ZipReader::OpenFile(const wxString &filename)
{
    m_file = new wxFFileInputStream(filename);
    m_zip = NULL;
    m_result = m_file;

    m_zipOk  = true;
    m_fileOk = m_file->IsOk();

    wxString zipText = wxEmptyString;
    wxString fileText = (filename.IsNull() ? filename : wxT(" ") + filename );
    m_info = wxString::Format(strBookNotFound, zipText.c_str(), fileText.c_str());
}

bool ZipReader::FindZip(wxFileName &zip_name, wxString &path)
{
    if (zip_name.FileExists()) return true;

    zip_name.SetPath(path);
    if (zip_name.FileExists()) return true;

    zip_name.SetPath(wxGetApp().GetAppPath());
    if (zip_name.FileExists()) return true;

    return false;
}

bool ZipReader::FindEntry(const wxString &file_name)
{
	bool find_ok = false;
	bool open_ok = false;
	while (wxZipEntry * entry = m_zip->GetNextEntry()) {
	    find_ok = (entry->GetName(wxPATH_UNIX) == file_name);
		if (find_ok) open_ok = m_zip->OpenEntry(*entry);
		delete entry;
		if (find_ok) break;
	}
	return find_ok && open_ok;
}

void ZipReader::ShowError()
{
    wxMessageBox(GetErrorText());
}

wxCriticalSection ZipCollection::sm_queue;

ZipCollection::ZipCollection() : m_id(1)
{
	m_tempfile = wxFileName::CreateTempFileName(wxT("~"));
	m_tempdata.Open(m_tempfile.GetFullPath());
	m_tempdata.ExecuteUpdate(wxT("CREATE TABLE books(book varchar(99), file integer);"));
	m_tempdata.ExecuteUpdate(wxT("CREATE TABLE files(file integer primary key, path text);"));
    m_tempdata.ExecuteUpdate(wxT("CREATE INDEX books_name ON books(book);"));
	m_tempdata.BeginTransaction();
}

ZipCollection::~ZipCollection()
{
	m_tempdata.RollBack();
	wxRemoveFile(m_tempfile.GetFullPath());
}

class ZipTraverser : public wxDirTraverser
{
public:
    ZipTraverser(ZipCollection* collection) : m_collection(collection) {};

    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
		wxString ext = filename.Right(4).Lower();
		if (ext == wxT(".zip")) m_collection->AddZip(filename);
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
        return wxDIR_IGNORE;
    }
private:
    ZipCollection* m_collection;
};

void ZipCollection::SetDir(const wxString &dirname)
{
	m_dirname = dirname;

    if (dirname.IsEmpty()) return;

    wxCriticalSectionLocker enter(sm_queue);

	m_tempdata.RollBack();
	m_tempdata.BeginTransaction();

	m_tempdata.ExecuteUpdate(wxT("DELETE FROM books;"));
	m_tempdata.ExecuteUpdate(wxT("DELETE FROM files;"));
	m_id = 1;

    wxDir dir(dirname);
    if ( !dir.IsOpened() ) return;

    {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
        event.SetString(dirname);
        event.SetInt(1);
        wxPostEvent( wxGetApp().GetTopWindow(), event );
    }

	ZipTraverser traverser(this);
	dir.Traverse(traverser);

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
        wxPostEvent( wxGetApp().GetTopWindow(), event );
	}
}

void ZipCollection::AddZip(const wxString &filename)
{
    wxFileName zip_file = filename;
	wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
    event.SetString(zip_file.GetFullName());
    event.SetInt(0);
	wxPostEvent( wxGetApp().GetTopWindow(), event );

	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	ZipFiles files(&m_tempdata);
	ZipFilesRow *file = files.New();
	file->file = m_id++;
	file->path = filename;
	file->Save();

	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			ZipBooks books(&m_tempdata);
			ZipBooksRow * book = books.New();
			book->book = entry->GetName(wxPATH_UNIX);
			book->file = file->file;
			book->Save();
		}
		delete entry;
	}
}

wxString ZipCollection::FindZip(const wxString &filename)
{
    wxCriticalSectionLocker enter(sm_queue);

	ZipBooks books(&m_tempdata);
	ZipBooksRow * book = books.Book(filename);
	if (book) {
		ZipFiles files(&m_tempdata);
		ZipFilesRow *file = files.File(book->file);
		if (file) return file->path;
	}
	return wxEmptyString;
}

