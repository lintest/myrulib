#include <wx/dir.h>
#include <wx/thread.h>
#include "BaseThread.h"
#include "ZipReader.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "db/ZipBooks.h"
#include "db/ZipFiles.h"
#include "FbManager.h"
#include "InfoCash.h"

extern wxString strBookNotFound;

class ZipThread : public BaseThread
{
	public:
		virtual void *Entry();
		void DoStep(const wxString &msg) { BaseThread::DoStep(msg); };
};

class ZipCollection {
	public:
		void Init(const wxString &dirname);
		wxString FindZip(const wxString &filename);
		void SetDir(const wxString &dirname);
		void AddZip(const wxString &filename);
	public:
		ZipThread * m_thread;
		static wxCriticalSection sm_queue;
	private:
		wxString m_dirname;
};

static ZipCollection zips;

void *ZipThread::Entry()
{
	wxString dir = FbParams::GetText(FB_LIBRARY_DIR);

    wxCriticalSectionLocker enter(zips.sm_queue);

	DoStart(0, dir);

	zips.m_thread = this;
	zips.SetDir(dir);

	DoFinish();

	InfoCash::Empty();

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

    wxDir dir(dirname);
    if ( !dir.IsOpened() ) return;

	ZipTraverser traverser(this);
	dir.Traverse(traverser);

}

void ZipCollection::AddZip(const wxString &filename)
{
    wxFileName zip_file = filename;
	m_thread->DoStep(zip_file.GetFullName());

	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	int id;
	{
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		ZipFiles files(wxGetApp().GetDatabase());
		ZipFilesRow * file = files.Path(zip_file.GetFullName());
		if (file) return;
		id = BookInfo::NewId(DB_NEW_ZIPFILE);
	}

    AutoTransaction trans;

	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
			ZipBooks books(wxGetApp().GetDatabase());
			ZipBooksRow * book = books.New();
			book->book = entry->GetName(wxPATH_UNIX);
			book->file = id;
			book->Save();
		}
		delete entry;
	}

	{
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		ZipFiles files(wxGetApp().GetDatabase());
		ZipFilesRow *file = files.New();
		file->file = id;
		file->path = zip_file.GetFullName();
		file->Save();
	}
}

wxString ZipCollection::FindZip(const wxString &filename)
{
    wxCriticalSectionLocker enter1(sm_queue);
	wxCriticalSectionLocker enter2(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT file FROM zip_books WHERE book=?");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, filename);
	DatabaseResultSet* result = pStatement->ExecuteQuery();

	if (!result) return wxEmptyString;

	while (result->Next()) {
		int id = result->GetResultInt(wxT("file"));
		ZipFiles files(wxGetApp().GetDatabase());
		ZipFilesRow *file = files.File(id);
		if (file) {
			wxFileName zip_file = file->path;
			zip_file.SetPath(m_dirname);
			if (zip_file.FileExists())
				return zip_file.GetFullPath();
		}
	}

	return wxEmptyString;
}

