#include "FbImportThread.h"
#include <wx/zipstrm.h>
#include <wx/dir.h>
#include <wx/list.h>
#include "FbConst.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "polarssl/md5.h"
#include "wx/base64.h"

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
	FbImportBook *ctx = (FbImportBook*)userData;
	wxString node_name = ctx->CharToLower(name);
	wxString path = ctx->Path();

	if (path == wxT("/fictionbook/description/title-info")) {
		if (node_name == wxT("author")) {
			ctx->author = new AuthorItem;
			ctx->authors.Add(ctx->author);
		} else if (node_name == wxT("sequence")) {
			SequenceItem * seqitem = new SequenceItem(atts);
			ctx->sequences.Add(seqitem);
		}
	}

	ctx->AppendTag(node_name);
	ctx->text.Empty();
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
	FbImportBook *ctx = (FbImportBook*)userData;
	wxString node_name = ctx->CharToLower(name);
	ctx->RemoveTag(node_name);
	wxString path = ctx->Path();

	if (path == wxT("/fictionbook/description/title-info")) {
		ctx->text.Trim(false).Trim(true);
		if (node_name == wxT("book-title")) {
			ctx->title = ctx->text;
		} else if (node_name == wxT("genre")) {
			ctx->genres += FbGenres::Char(ctx->text);
		}
	} else if (path == wxT("/fictionbook/description/title-info/author")) {
		ctx->text.Trim(false).Trim(true);
		if (node_name == wxT("first-name"))
			ctx->author->first = ctx->text;
		if (node_name == wxT("middle-name"))
			ctx->author->middle = ctx->text;
		if (node_name == wxT("last-name"))
			ctx->author->last = ctx->text;
	} else if (path == wxT("/fictionbook/description/publish-info/")) {
		if (node_name == wxT("isbn"))
			ctx->isbn = ctx->text.Trim(true).Trim(false);
	} else if (path == wxT("/fictionbook/description")) {
		if (node_name == wxT("title-info")) {
			ctx->Stop();
		}
	}
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
	FbImportBook *ctx = (FbImportBook*)userData;
	wxString str = ctx->CharToString(s, len);
	if (!ctx->IsWhiteOnly(str)) ctx->text += str;
}
}

//-----------------------------------------------------------------------------
//  FbImportBook
//-----------------------------------------------------------------------------

FbImportBook::FbImportBook(FbDatabase &database, const wxString &message, const wxString &md5sum)
	: m_database(database), m_message(message), m_md5sum(md5sum)
{
}

bool FbImportBook::Load(wxInputStream& stream)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	md5_context md5;
	if (m_md5sum.IsEmpty()) md5_starts( &md5 );

	XML_SetElementHandler(GetParser(), StartElementHnd, EndElementHnd);
	XML_SetCharacterDataHandler(GetParser(), TextHnd);

	bool done;
	bool ok = true;
	bool skip = false;

	do {
		size_t len = stream.Read(buf, BUFSIZE).LastRead();
		done = (len < BUFSIZE);

		if (m_md5sum.IsEmpty()) md5_update( &md5, buf, (int) len );

		if (!skip) {
			if ( !XML_Parse(GetParser(), (char *)buf, len, done) ) {
				XML_Error error_code = XML_GetErrorCode(GetParser());
				if ( error_code == XML_ERROR_ABORTED ) {
					skip = true;
				} else {
					wxString error(XML_ErrorString(error_code), *wxConvCurrent);
					wxLogError(_("XML parsing error: '%s' at line %d file %s"), error.c_str(), XML_GetCurrentLineNumber(GetParser()), m_message.c_str());
					skip = true;
					ok = false;
					break;
				}
			}
		}
	} while (!done);

	if (m_md5sum.IsEmpty()) m_md5sum = BaseThread::CalcMd5(md5);

	return ok;
}

wxString FbImportBook::CalcMd5(wxInputStream& stream)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	md5_context md5;
	md5_starts( &md5 );

	bool done;
	do {
		size_t len = stream.Read(buf, BUFSIZE).LastRead();
		done = (len < BUFSIZE);
		md5_update( &md5, buf, (int) len );
	} while (!done);

	return BaseThread::CalcMd5(md5);
}

void FbImportBook::Convert()
{
	for (size_t i=0; i<authors.Count(); i++)
		authors[i].Convert(m_database);

	if (authors.Count() == 0)
		authors.Add(new AuthorItem);

	for (size_t i=0; i<sequences.Count(); i++)
		sequences[i].Convert(m_database);
}

int FbImportBook::FindByMD5()
{
	wxString sql = wxT("SELECT id FROM books WHERE md5sum=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, m_md5sum);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0) : 0;
}

int FbImportBook::FindBySize(wxFileOffset size)
{
	wxArrayInt books;
	{
		wxString sql = wxT("SELECT DISTINCT id FROM books WHERE file_size=? AND (md5sum='' OR md5sum IS NULL)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, (wxLongLong)size);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		while (result.NextRow()) books.Add(result.GetInt(0));
	}
	for (size_t i=0; i<books.Count(); i++) {
		int id = books[i];
		ZipReader book(id);
		if (!book.IsOK()) continue;
		wxString md5sum = FbImportBook::CalcMd5(book.GetZip());
		wxString sql = wxT("UPDATE books SET md5sum=? WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, md5sum);
		stmt.Bind(2, id);
		stmt.ExecuteUpdate();
		if (m_md5sum == md5sum) return id;
	}
	return 0;
}

void FbImportBook::AppendBook(const wxString &filename, wxFileOffset size, int id_archive)
{
	Convert();

	int id_book = - m_database.NewId(DB_NEW_BOOK);
	long today = 0;
	wxDateTime::Now().Format(wxT("%y%m%d")).ToLong(&today);

	for (size_t i = 0; i<authors.Count(); i++) {
		{
			wxString sql = wxT("INSERT INTO books(id,id_archive,id_author,title,genres,file_name,file_size,file_type,created,md5sum) VALUES (?,?,?,?,?,?,?,?,?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, id_archive);
			stmt.Bind(3, authors[i].id);
			stmt.Bind(4, title);
			stmt.Bind(5, genres);
			stmt.Bind(6, filename);
			stmt.Bind(7, (wxLongLong)size);
			stmt.Bind(8, wxFileName(filename).GetExt().Lower());
			stmt.Bind(9, (int) today);
			stmt.Bind(10, m_md5sum);
			stmt.ExecuteUpdate();
		}
	}

	for (size_t i = 0; i<sequences.Count(); i++) {
		if (sequences[i].id == 0) continue;
		wxString sql = wxT("INSERT INTO bookseq(id_book,id_seq,number) VALUES (?,?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id_book);
		stmt.Bind(2, sequences[i].id);
		stmt.Bind(3, (int)sequences[i].number);
		stmt.ExecuteUpdate();
	}

}
void FbImportBook::AppendFile(int id_book, const wxString &filename, int id_archive)
{
	wxString sql = wxT("SELECT file_name FROM books WHERE id=? AND id_archive=? UNION SELECT file_name FROM files WHERE id_book=? AND id_archive=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id_book);
	stmt.Bind(2, id_archive);
	stmt.Bind(3, id_book);
	stmt.Bind(4, id_archive);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	while (result.NextRow()) {
		if (result.GetString(0) == filename) {
			wxLogWarning(_("File already exists %s"), m_message.c_str());
			return;
		}
	}

	wxLogWarning(_("Add alternative %s"), filename.c_str());
	{
		wxString sql = wxT("INSERT INTO files(id_book, id_archive, file_name) VALUES (?,?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id_book);
		stmt.Bind(2, id_archive);
		stmt.Bind(3, filename);
		stmt.ExecuteUpdate();
	}
}

void FbImportBook::Save(const wxString &filename, wxFileOffset filesize, int id_archive)
{
	try {
		int id_book = FindByMD5();
		if (!id_book) id_book = FindBySize(filesize);
		if (id_book)
			AppendFile(id_book, filename, id_archive);
		else
			AppendBook(filename, filesize, id_archive);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

//-----------------------------------------------------------------------------
//  FbImportThread
//-----------------------------------------------------------------------------

FbImportThread::FbImportThread()
	: m_basepath(FbParams::GetText(DB_LIBRARY_DIR))
{
}

void FbImportThread::OnExit()
{
	m_database.ExecuteUpdate(strUpdateAuthorCount);
	m_database.ExecuteUpdate(strUpdateSequenCount);
}

wxString FbImportThread::GetRelative(const wxString &filename)
{
	wxFileName result = filename;
	result.Normalize(wxPATH_NORM_ALL);
	result.MakeRelativeTo(m_basepath);
	return result.GetFullPath(wxPATH_UNIX);
}

int FbImportThread::AppendZip(const wxString &filename, const int size, const int count)
{
	{
		wxString sql = wxT("SELECT id FROM archives WHERE file_name=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, filename);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return result.GetInt(0);
	}

	int id = m_database.NewId(DB_NEW_ARCHIVE);
	{
		wxString sql = wxT("INSERT INTO archives(id, file_name, file_size, file_count) VALUES (?,?,?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id);
		stmt.Bind(2, filename);
		stmt.Bind(3, size);
		stmt.Bind(4, (wxLongLong)count);
		stmt.ExecuteUpdate();
	}
	return id;
}

void *FbZipImportThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	size_t count = m_filelist.Count();
	wxLogInfo(_("Start import %d file(s)"), count);
	for (size_t i=0; i<count; i++) {
		ImportFile(m_filelist[i]);
	}
	wxLogInfo(_("Finish import %d file(s)"), count);
	return NULL;
}

WX_DECLARE_STRING_HASH_MAP(wxZipEntry*, FbZipEntryMap);

WX_DECLARE_OBJARRAY(wxZipEntry*, FbZipEntryList);

WX_DEFINE_OBJARRAY(FbZipEntryList);

class FbZipCatalog
{
	public:
		FbZipCatalog(wxZipInputStream &zip);
		size_t Count() { return m_list.Count(); };
		wxZipEntry * GetNext();
		wxZipEntry * GetInfo(const wxString & filename);
	private:
		FbZipEntryList m_list;
		FbZipEntryMap m_map;
		size_t m_pos;
};

FbZipCatalog::FbZipCatalog(wxZipInputStream &zip)
	:m_pos(0)
{
    while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetInternalName();
			if (filename.Right(4).Lower() == wxT(".fbd")) {
				filename = filename.Left(filename.Length() - 4);
				wxZipEntry*& current = m_map[filename];
				delete current;
				current = entry;
			} else {
				m_list.Add(entry);
			}
		}
    }
}

wxZipEntry * FbZipCatalog::GetNext()
{
	if (m_pos >= m_list.Count()) return NULL;
	return m_list[m_pos++];
}

wxZipEntry * FbZipCatalog::GetInfo(const wxString & filename)
{
	size_t pos = filename.Length();
	while (pos) {
		if ( filename[pos] == wxT('.') ) {
			wxString infoname = filename.Left(pos);
			FbZipEntryMap::iterator it = m_map.find(infoname);
			if (it != m_map.end()) return it->second;
			break;
		}
		pos--;
	}
	return NULL;
}

void FbZipImportThread::ImportFile(const wxString & zipname)
{
	wxLogInfo(_("Import file %s"), zipname.c_str());

	wxFFileInputStream in(zipname);
	if ( !in.IsOk() ){
		wxLogError(wxT("File read error %s"), zipname.c_str());
		return;
	}

	FbAutoCommit transaction(m_database);

	wxString filename = GetRelative(zipname);

	if (zipname.Right(4).Lower() == wxT(".fb2")) {
		DoStart(0, zipname);
		FbImportBook book(m_database, zipname);
		if (book.Load(in)) book.Save(filename, in.GetLength());
		DoFinish();
		return;
	}

	wxCSConv conv(wxT("cp866"));
	wxZipInputStream zip(in, conv);
	if ( !zip.IsOk() ){
		wxLogError(wxT("Zip read error %s"), zipname.c_str());
		return;
	}

	DoStart(0, zipname);
	int id_archive = AppendZip(filename, in.GetLength(), zip.GetTotalEntries());

    FbZipCatalog cat(zip);
	size_t existed = cat.Count();
	DoStart(existed, zipname);

	size_t skipped = 0;
	while (wxZipEntry * entry = cat.GetNext()) {
		wxString filename = entry->GetInternalName();
		DoStep(filename);
		wxString md5sum;
		if (filename.Right(4).Lower() == wxT(".fb2")) {
			zip.OpenEntry(*entry);
		} else {
			wxZipEntry * info = cat.GetInfo(filename);
			if (info) {
				zip.OpenEntry(*entry);
				md5sum = FbImportBook::CalcMd5(zip);
				zip.OpenEntry(*info);
			} else {
				wxLogWarning(_("Skip file %s"), filename.c_str());
				skipped++;
				continue;
			}
		}
		wxLogInfo(_("Import zip entry %s"), filename.c_str());
		FbImportBook book(m_database, zipname + wxT(": ") + filename);
		if (book.Load(zip)) book.Save(filename, entry->GetSize(), id_archive);
	}

	if ( existed && skipped ) wxLogWarning(wxT("FB2 and FBD not found %s"), zipname.c_str());
	if ( !existed ) wxLogError(wxT("Zip read error %s"), zipname.c_str());

	DoFinish();
}

class CountTraverser : public wxDirTraverser
{
public:
	CountTraverser() : m_count(0) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		wxString ext = filename.Right(4).Lower();
		if (ext== wxT(".fb2")) {
			m_count++;
		} else if (ext== wxT(".zip")) {
			m_count++;
		}
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) {
		return wxDIR_CONTINUE;
	}
	unsigned int GetCount() { return m_count; }
private:
	unsigned int m_count;
};

class FolderTraverser : public wxDirTraverser
{
public:
	FolderTraverser(FbDirImportThread* thread) : m_thread(thread), m_progress(0) { };

	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		wxString ext = filename.Right(4).Lower();
		if (ext == wxT(".fb2")) {
			Progress(filename);
			wxLogInfo(_("Import file %s"), filename.c_str());
			m_thread->ParseXml(filename);
		} else if (ext == wxT(".zip")) {
			Progress(filename);
			wxLogInfo(_("Import file %s"), filename.c_str());
			m_thread->ParseZip(filename);
		} else {
			wxLogWarning(_("Skip file %s"), filename.c_str());
		}
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& dirname)  {
		wxLogInfo(_("Import subdirectory %s"), dirname.c_str());
		return wxDIR_CONTINUE;
	}
private:
	void Progress(const wxString &filename) {
		m_thread->DoStep( wxFileName(filename).GetFullName() );
	}
private:
	FbDirImportThread *m_thread;
	unsigned int m_progress;
};

void *FbDirImportThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	wxLogInfo(_("Start import directory %s"), m_dirname.c_str());

	wxDir dir(m_dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), m_dirname.c_str());
		return NULL;
	}

	{
		DoStart(0, m_dirname);
		CountTraverser counter;
		dir.Traverse(counter);
		DoStart(counter.GetCount(), m_dirname);
	}

	FolderTraverser traverser(this);
	dir.Traverse(traverser);

	DoFinish();

	wxLogInfo(_("Finish import directory %s"), m_dirname.c_str());

	return NULL;
}

void FbDirImportThread::ParseXml(const wxString &filename)
{
	FbAutoCommit transaction(m_database);
	wxFFileInputStream in(filename);
	FbImportBook book(m_database, filename);
	if (book.Load(in)) book.Save(GetRelative(filename), in.GetLength());
}

void FbDirImportThread::ParseZip(const wxString &zipname)
{
	wxFFileInputStream in(zipname);
	if ( !in.IsOk() ){
		wxLogError(wxT("Zip read error %s"), zipname.c_str());
		return;
	}

	wxCSConv conv(wxT("cp866"));
	wxZipInputStream zip(in, conv);
	if ( !zip.IsOk() ){
		wxLogError(wxT("Zip read error %s"), zipname.c_str());
		return;
	}

	FbAutoCommit transaction(m_database);

	wxString filename = GetRelative(zipname);
	int id_archive = AppendZip(filename, in.GetLength(), zip.GetTotalEntries());

    FbZipCatalog cat(zip);
	size_t existed = cat.Count();
	size_t skipped = 0;
	while (wxZipEntry * entry = cat.GetNext()) {
		wxString filename = entry->GetInternalName();
		wxString md5sum;
		if (filename.Right(4).Lower() == wxT(".fb2")) {
			zip.OpenEntry(*entry);
		} else {
			wxZipEntry * info = cat.GetInfo(filename);
			if (info) {
				zip.OpenEntry(*entry);
				md5sum = FbImportBook::CalcMd5(zip);
				zip.OpenEntry(*info);
			} else {
				wxLogWarning(_("Skip file %s"), filename.c_str());
				skipped++;
				continue;
			}
		}
		wxLogInfo(_("Import zip entry %s"), filename.c_str());
		FbImportBook book(m_database, zipname + wxT(": ") + filename);
		if (book.Load(zip)) book.Save(filename, entry->GetSize(), id_archive);
	}

	if ( existed && skipped ) wxLogWarning(wxT("FB2 and FBD not found %s"), zipname.c_str());
	if ( !existed ) wxLogError(wxT("Zip read error %s"), zipname.c_str());
}
