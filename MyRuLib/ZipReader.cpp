#include <wx/dir.h>
#include "ZipReader.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "db/ZipBooks.h"
#include "db/ZipFiles.h"

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

ZipReader::ZipReader(int id)
    :m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
    {
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

        wxString path = FbParams::GetText(FB_LIBRARY_DIR);

        Books books(wxGetApp().GetDatabase());
        BooksRow * bookRow = books.Id(id);

        if (!bookRow) return;

        m_file_name = bookRow->file_name;

        if (bookRow->id_archive) {
            ArchivesRow * archiveRow = bookRow->GetArchive();
            if (!archiveRow) return;
            m_zip_name = archiveRow->file_name;
            wxFileName zip_file = m_zip_name;
            zip_file.SetPath(archiveRow->file_path);
            m_zipOk = FindZip(zip_file, path);
            if (m_zipOk) OpenZip(zip_file.GetFullPath());
        } else if (wxFileName::FileExists(bookRow->file_name)) {
			OpenFile(bookRow->file_name);
		} else {
			m_zip_name = zips.FindZip(bookRow->file_name);
			if (m_zip_name.IsEmpty()) return;
			wxFileName zip_file = m_zip_name;
			m_zipOk = FindZip(zip_file, path);
			if (m_zipOk) OpenZip(zip_file.GetFullPath());
        }
    }
}

ZipReader::~ZipReader()
{
	wxDELETE(m_zip);
	wxDELETE(m_file);
}

void ZipReader::Init()
{
	zips.SetDir(FbParams::GetText(FB_LIBRARY_DIR));
}

void ZipReader::OpenZip(const wxString &zipname)
{
    m_file = new wxFFileInputStream(zipname);
    m_zip = new wxZipInputStream(*m_file);
    m_result = m_zip;

    m_zipOk  = m_file->IsOk();
    m_fileOk = FindEntry(m_file_name);
}

void ZipReader::OpenFile(const wxString &filename)
{
    m_file = new wxFFileInputStream(filename);
    m_zip = NULL;
    m_result = m_file;

    m_zipOk  = true;
    m_fileOk = m_file->IsOk();
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

extern wxString strBookNotFound;

void ZipReader::ShowError()
{
    wxMessageBox(GetErrorText());
}

wxString ZipReader::GetErrorText()
{
    wxString zipText = (m_zip_name.IsNull() ? m_zip_name : wxT(" ") + m_zip_name );
    wxString fileText = (m_file_name.IsNull() ? m_file_name : wxT(" ") + m_file_name );
    return wxString::Format(strBookNotFound, zipText.c_str(), fileText.c_str());
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
        return wxDIR_CONTINUE;
    }
private:
    ZipCollection* m_collection;
};

void ZipCollection::SetDir(const wxString &dirname)
{
	m_dirname = dirname;

    wxCriticalSectionLocker enter(sm_queue);

	m_tempdata.RollBack();
	m_tempdata.BeginTransaction();

	m_tempdata.ExecuteUpdate(wxT("DELETE FROM books;"));
	m_tempdata.ExecuteUpdate(wxT("DELETE FROM files;"));
	m_id = 1;

    wxDir dir(dirname);
    if ( !dir.IsOpened() ) return;

	ZipTraverser traverser(this);
	dir.Traverse(traverser);

}

void ZipCollection::AddZip(const wxString &filename)
{
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

