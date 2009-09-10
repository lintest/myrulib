#include <wx/dir.h>
#include <wx/thread.h>
#include "BaseThread.h"
#include "ZipReader.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "db/Files.h"
#include "db/ZipBooks.h"
#include "db/ZipFiles.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "FbConst.h"

class ZipThread : public BaseThread
{
	public:
		ZipThread(const wxString &dirname): BaseThread(), m_dirname(dirname) {};
		virtual void *Entry();
		void DoStep(const wxString &msg) { BaseThread::DoStep(msg); };
	private:
		wxString m_dirname;
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
    wxCriticalSectionLocker enter(zips.sm_queue);

	DoStart(0, m_dirname);

	zips.m_thread = this;
	zips.SetDir(m_dirname);

	DoFinish();

	return NULL;
}

class ExtractItems
{
	public:
		ExtractItems(BooksRow * book)
			:id_book(book->id), id_archive(book->id_archive), book_name(book->file_name) {};
		ExtractItems(FilesRow * file)
			:id_book(file->id_book), id_archive(file->id_archive), book_name(file->file_name) {};
	public:
		int id_book;
		int id_archive;
		wxString book_name;
		wxFileName zip_name;
};

WX_DECLARE_OBJARRAY(ExtractItems, ExtractInfoArray);

WX_DEFINE_OBJARRAY(ExtractInfoArray);

ZipReader::ZipReader(int id)
    :m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
	ExtractInfoArray items;

    {
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

        Books books(wxGetApp().GetDatabase());
        BooksRow * bookRow = books.Id(id);
        if (!bookRow) return;

        items.Add(bookRow);

        Files files(wxGetApp().GetDatabase());
        FilesRowSet * fileRows = files.IdBook(bookRow->id);
        for (size_t i = 0; i<fileRows->Count(); i++) {
			items.Add(fileRows->Item(i));
        }

        for (size_t i = 0; i<items.Count(); i++) {
        	ExtractItems & item = items[i];
        	if ( !item.id_archive ) continue;
			Archives archives(wxGetApp().GetDatabase());
			ArchivesRow * archiveRow = archives.Id(item.id_archive);
			if ( !archiveRow ) continue;
			item.zip_name = archiveRow->file_name;
			item.zip_name.SetPath(archiveRow->file_path);
        }
    }

	wxString sLibraryDir = FbParams::GetText(FB_LIBRARY_DIR);
	wxString sWanraikDir = FbParams::GetText(FB_WANRAIK_DIR);

	for (size_t i = 0; i<items.Count(); i++) {
		ExtractItems & item = items[i];
		if (item.id_archive) {
			m_zipOk = item.zip_name.FileExists();
			if (!m_zipOk) {
				item.zip_name.SetPath(sLibraryDir);
				m_zipOk = item.zip_name.FileExists();
			}
			if (!m_zipOk) {
				item.zip_name.SetPath(sWanraikDir);
				m_zipOk = item.zip_name.FileExists();
			}
			if (m_zipOk) OpenZip(item.zip_name.GetFullPath(), item.book_name);
		} else if (item.id_book > 0) {
			wxString zip_name = zips.FindZip(item.book_name);
			m_zipOk = !zip_name.IsEmpty();
			if (m_zipOk) OpenZip(zip_name, item.book_name);
		} else if (wxFileName::FileExists(item.book_name)) {
			OpenFile(item.book_name);
		}
		if (IsOK()) return;
	}
}

ZipReader::~ZipReader()
{
	wxDELETE(m_zip);
	wxDELETE(m_file);
}

void ZipReader::Init()
{
	wxString dirname = FbParams::GetText(FB_LIBRARY_DIR);
	ZipThread *thread = new ZipThread(dirname);

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

    wxLogInfo(_("Start scan directory %s"), m_dirname.c_str());

    wxDir dir(dirname);
    if ( !dir.IsOpened() ) {
        wxLogError(_("Can't open directory %s"), m_dirname.c_str());
        return;
    }

	ZipTraverser traverser(this);
	dir.Traverse(traverser);

    wxLogInfo(_("Finish scan directory %s"), m_dirname.c_str());
}

void ZipCollection::AddZip(const wxString &filename)
{
    wxLogInfo(_("Scan zip %s"), filename.c_str());

    wxFileName zip_file = filename;
	m_thread->DoStep(zip_file.GetFullName());

	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	int id = 0;
	{
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		ZipFiles files(wxGetApp().GetDatabase());
		ZipFilesRow * file = files.Path(zip_file.GetFullName());
		if (file) return;
		id = BookInfo::NewId(DB_NEW_ZIPFILE);
	}

    AutoTransaction trans;

    int count = 0;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
			ZipBooks books(wxGetApp().GetDatabase());
			ZipBooksRow * book = books.New();
			book->book = entry->GetName(wxPATH_UNIX);
			book->file = id;
			book->Save();
			count++;
		}
		delete entry;
	}

	if (count) {
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		ZipFiles files(wxGetApp().GetDatabase());
		ZipFilesRow *file = files.New();
		file->file = id;
		file->path = zip_file.GetFullName();
		file->Save();
	}

	InfoCash::Empty();
}

wxString ZipCollection::FindZip(const wxString &filename)
{
    wxCriticalSectionLocker enter1(sm_queue);
	wxCriticalSectionLocker enter2(wxGetApp().m_DbSection);

    DatabaseLayer * database = wxGetApp().GetDatabase();
	wxString sql = wxT("SELECT file FROM zip_books WHERE book=?");
	PreparedStatement* pStatement = database->PrepareStatement(sql);
	pStatement->SetParamString(1, filename);
	DatabaseResultSet* result = pStatement->ExecuteQuery();

	if (!result) return wxEmptyString;

    wxString zipname;
	while (result && result->Next()) {
		int id = result->GetResultInt(wxT("file"));
		ZipFiles files(wxGetApp().GetDatabase());
		ZipFilesRow *file = files.File(id);
		if (file) {
			wxFileName zip_file = file->path;
			zip_file.SetPath(m_dirname);
			if (zip_file.FileExists()) {
				zipname = zip_file.GetFullPath();
				break;
			}
		}
	}

	database->CloseResultSet(result);
	database->CloseStatement(pStatement);

	return zipname;
}

