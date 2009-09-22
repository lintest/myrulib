#include <wx/dir.h>
#include <wx/thread.h>
#include "BaseThread.h"
#include "ZipReader.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "FbConst.h"
#include "BookExtractInfo.h"

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

ZipReader::ZipReader(int id, bool bShowError)
    :conv(wxT("cp866")), m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
	BookExtractInfoArray items;
	wxString file_name;

    {
        wxString sql = wxT("\
            SELECT DISTINCT 0 AS Key, id, id_archive, file_name, file_path FROM books WHERE id=? UNION ALL \
            SELECT DISTINCT 1 AS Key, id_book, id_archive, file_name, file_path FROM files WHERE id_book=? \
            ORDER BY Key \
        ");

        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
        wxSQLite3Statement stmt = wxGetApp().GetDatabase().PrepareStatement(sql);
        stmt.Bind(1, id);
        stmt.Bind(2, id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        while ( result.NextRow() ) items.Add(result);
    }

    {
        wxString sql = wxT("SELECT file_name, file_path FROM archives WHERE id=?");
        for (size_t i = 0; i<items.Count(); i++) {
            BookExtractInfo & item = items[i];
            if (!item.id_archive) continue;
            wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
            wxSQLite3Statement stmt = wxGetApp().GetDatabase().PrepareStatement(sql);
            stmt.Bind(1, item.id_archive);
            wxSQLite3ResultSet result = stmt.ExecuteQuery();
            if (result.NextRow()) {
                item.zip_name = result.GetString(wxT("file_name"));
                item.zip_path = result.GetString(wxT("file_path"));
            }
        }
    }

	wxString sLibraryDir = FbParams::GetText(FB_LIBRARY_DIR);
	wxString sWanraikDir = FbParams::GetText(FB_WANRAIK_DIR);

	for (size_t i = 0; i<items.Count(); i++) {
		BookExtractInfo & item = items[i];
		if (item.id_archive) {
		    wxFileName zip_file = item.GetZip();
			m_zipOk = zip_file.FileExists();
			if (!m_zipOk) m_zipOk = (zip_file = item.GetZip(sLibraryDir)).FileExists();
			if (!m_zipOk) m_zipOk = (zip_file = item.GetZip(sWanraikDir)).FileExists();
			if (m_zipOk) OpenZip(zip_file.GetFullPath(), item.book_name);
		} else if (item.id_book > 0) {
			wxString zip_name = zips.FindZip(item.book_name);
			m_zipOk = !zip_name.IsEmpty();
			if (m_zipOk) OpenZip(zip_name, item.book_name);
		} else {
		    wxFileName book_file = item.GetBook();
		    if (book_file.FileExists(item.book_name)) OpenFile(book_file.GetFullPath());
		}
		if (IsOK()) return;
	}
	if (bShowError) wxLogError(_("Book open error (%d) not found %s"), id, file_name.c_str());
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
    m_zip = new wxZipInputStream(*m_file, conv);
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
	    find_ok = (entry->GetInternalName() == file_name);
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

	wxSQLite3Database & database = wxGetApp().GetDatabase();

	int id = 0;
	{
        wxString sql = wxT("SELECT file FROM zip_files WHERE path=?");
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, filename);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) return ;
		id = BookInfo::NewId(DB_NEW_ZIPFILE);
	}

    wxSQLite3Transaction trans(&database);

    int count = 0;
    {
        wxString sql = wxT("INSERT INTO zip_books(file,book) values(?,?)");
        while (wxZipEntry * entry = zip.GetNextEntry()) {
            if (entry->GetSize()) {
                wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
                wxSQLite3Statement stmt = database.PrepareStatement(sql);
                stmt.Bind(1, id);
                stmt.Bind(2, entry->GetName(wxPATH_UNIX));
                stmt.ExecuteUpdate();
                count++;
            }
            delete entry;
        }
    }

	if (count) {
        wxString sql = wxT("INSERT INTO zip_files(file,path) values(?,?)");
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, id);
        stmt.Bind(2, filename);
        stmt.ExecuteUpdate();
        trans.Commit();
	}

	InfoCash::Empty();
}

wxString ZipCollection::FindZip(const wxString &filename)
{
    wxCriticalSectionLocker enter1(sm_queue);
	wxCriticalSectionLocker enter2(wxGetApp().m_DbSection);

	wxSQLite3Database & database = wxGetApp().GetDatabase();

    wxString sql = wxT("SELECT file FROM zip_books WHERE book=?");
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, filename);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

    while (result.NextRow())  {
        wxString sql = wxT("SELECT path FROM zip_files WHERE file=?");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, result.GetInt(0));
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) {
            wxFileName zip_file = result.GetString(0);
            zip_file.SetPath(m_dirname);
            if (zip_file.FileExists()) return zip_file.GetFullPath();
        }
    }

    return wxEmptyString;
}

