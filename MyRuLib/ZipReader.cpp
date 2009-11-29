#include <wx/dir.h>
#include <wx/thread.h>
#include "BaseThread.h"
#include "ZipReader.h"
#include "FbParams.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "FbConst.h"
#include "BookExtractInfo.h"
#include "FbDatabase.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbDownloader.h"

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
		void AddZip(FbCommonDatabase & database, const wxString &filename);
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

	wxSleep(3);

	DoStart(0, m_dirname);

	zips.m_thread = this;
	zips.SetDir(m_dirname);

	InfoCash::Empty();
	DoFinish();

	return NULL;
}

ZipReader::ZipReader(int id, bool bShowError, bool bInfoOnly)
	:conv(wxT("cp866")), m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
	FbCommonDatabase database;

	OpenDownload(database);
	if (IsOK()) return;

	BookExtractArray items(database, id);

	wxString file_name;
	wxString sLibraryDir = FbParams::GetText(DB_LIBRARY_DIR);

	for (size_t i = 0; i<items.Count(); i++) {
		BookExtractInfo & item = items[i];
		if (item.id_archive) {
			if ( bInfoOnly && (item.book_name.Right(4).Lower()!=wxT(".fb2")) )
				item.book_name = GetInfoName(item.book_name);
			wxFileName zip_file = item.GetZip(sLibraryDir);
			m_zipOk = zip_file.FileExists();
			if (m_zipOk) OpenZip(zip_file.GetFullPath(), item.book_name);
		} else if (item.librusec) {
			wxString zip_name = zips.FindZip(item.book_name);
			if (!zip_name.IsEmpty()) {
				wxFileName zip_file = zip_name;
				zip_file.SetPath(sLibraryDir);
				m_zipOk = zip_file.FileExists();
				if (m_zipOk) OpenZip(zip_file.GetFullPath(), item.book_name);
			}
		} else {
			wxFileName book_file = item.GetBook(sLibraryDir);
			if (book_file.FileExists()) OpenFile(book_file.GetFullPath());
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

wxString ZipReader::GetInfoName(const wxString &filename)
{
	wxString result = filename;
	size_t pos = filename.Length();
	while (pos) {
		if ( filename[pos] == wxT('.') ) {
			result = result.Left(pos);
			break;
		}
		pos--;
	}
	return result + wxT(".fbd");
}

void ZipReader::Init()
{
	wxString dirname = FbParams::GetText(DB_LIBRARY_DIR);
	wxThread *thread = new ZipThread(dirname);

	if ( thread->Create() != wxTHREAD_NO_ERROR ) {
		wxLogError(wxT("Can't create thread!"));
		return;
	}

	thread->Run();

	return;
}

void ZipReader::OpenDownload(FbDatabase &database)
{
	wxString md5sum;

	{
		wxString sql = wxT("SELECT md5sum FROM books WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if ( result.NextRow() ) md5sum = result.GetString(0);
		else return;
	}

	wxFileName zip_file = FbDownloader::GetFilename(md5sum, false);
	m_zipOk = zip_file.FileExists();
	if (m_zipOk) {
		m_file = new wxFFileInputStream(zip_file.GetFullPath());
		m_zip = NULL;
		m_result = m_file;
		m_fileOk = true;
		return;
	}

	zip_file.SetExt(wxT("zip"));
	m_zipOk = zip_file.FileExists();
	if (m_zipOk) {
		m_file = new wxFFileInputStream(zip_file.GetFullPath());
		m_zip = new wxZipInputStream(*m_file, conv);
		m_result = m_zip;
		if (wxZipEntry * entry = m_zip->GetNextEntry()) {
			m_fileOk = m_zip->OpenEntry(*entry);
			delete entry;
		}
	}
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
		if (ext == wxT(".zip")) m_collection->AddZip(m_database, filename);
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
	{
		return wxDIR_IGNORE;
	}
private:
	ZipCollection* m_collection;
	FbCommonDatabase m_database;
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

void ZipCollection::AddZip(FbCommonDatabase & database, const wxString &filename)
{
	wxLogInfo(_("Scan zip %s"), filename.c_str());

	wxFileName zip_file = filename;
	m_thread->DoStep(zip_file.GetFullName());

	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	int id = 0;
	{
		wxString sql = wxT("SELECT file FROM zip_files WHERE path=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, filename);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return ;
		id = database.NewId(DB_NEW_ZIPFILE);
	}

	FbAutoCommit transaction(database);

	int count = 0;
	{
		wxString sql = wxT("INSERT INTO zip_books(file,book) values(?,?)");
		while (wxZipEntry * entry = zip.GetNextEntry()) {
			if (entry->GetSize()) {
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
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		stmt.Bind(2, filename);
		stmt.ExecuteUpdate();
	} else {
		wxLogError(wxT("Zip read error %s"), filename.c_str());
	}

	InfoCash::Empty();
}

wxString ZipCollection::FindZip(const wxString &filename)
{
	FbCommonDatabase database;

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
