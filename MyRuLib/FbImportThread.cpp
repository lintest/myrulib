#include "FbImportThread.h"
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
		} else if (node_name == wxT("lang")) {
			ctx->lang = ctx->text.Lower();
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

FbImportBook::FbImportBook(FbImportThread *owner, wxInputStream &in, const wxString &filename):
	m_database(owner->m_database),
	m_filename(owner->GetRelative(filename)),
	m_filepath(owner->GetAbsolute(filename)),
	m_message(filename),
	m_filesize(in.GetLength()),
	m_archive(0),
	m_ok(false)
{
	wxLogInfo(_("Import file %s"), m_filename.c_str());
	m_ok = Load(in);
}

FbImportBook::FbImportBook(FbImpotrZip *owner, wxZipEntry *entry):
	m_database(owner->m_database),
	m_filename(entry->GetInternalName()),
	m_message(owner->m_filename + wxT(": ") + m_filename),
	m_filesize(entry->GetSize()),
	m_archive(owner->m_id),
	m_ok(false)
{
	wxString md5sum;
	if (m_filename.Right(4).Lower() == wxT(".fb2")) {
		owner->OpenEntry(*entry);
	} else {
		wxZipEntry * info = owner->GetInfo(m_filename);
		if (info) {
			owner->OpenEntry(*entry);
			md5sum = CalcMd5(owner->m_zip);
			owner->OpenEntry(*info);
		} else {
			wxLogWarning(_("Skip entry %s"), m_filename.c_str());
			m_ok = false;
			return;
		}
	}
	wxLogInfo(_("Import zip entry %s"), m_filename.c_str());
	m_ok = Load(owner->m_zip);
}

bool FbImportBook::Load(wxInputStream& stream)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	md5_context md5cont;
	bool dm5empty = m_md5sum.IsEmpty();
	if (dm5empty) md5_starts( &md5cont );

	XML_SetElementHandler(GetParser(), StartElementHnd, EndElementHnd);
	XML_SetCharacterDataHandler(GetParser(), TextHnd);

	bool ok = true;
	bool parse = true;
	bool eof;

	do {
		size_t len = stream.Read(buf, BUFSIZE).LastRead();
		eof = (len < BUFSIZE);

		if (dm5empty) md5_update( &md5cont, buf, (int) len );

		if (parse) {
			if ( !XML_Parse(GetParser(), (char *)buf, len, eof) ) {
				XML_Error error_code = XML_GetErrorCode(GetParser());
				if ( error_code == XML_ERROR_ABORTED ) {
					parse = false;
				} else {
					wxString error(XML_ErrorString(error_code), *wxConvCurrent);
					wxLogError(_("XML parsing error: '%s' at line %d file %s"), error.c_str(), XML_GetCurrentLineNumber(GetParser()), m_message.c_str());
					parse = false;
					ok = false;
					break;
				}
			}
		}
	} while (!eof);

	if (dm5empty) m_md5sum = BaseThread::CalcMd5(md5cont);

	return ok;
}

wxString FbImportBook::CalcMd5(wxInputStream& stream)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	md5_context md5;
	md5_starts( &md5 );

	bool eof;
	do {
		size_t len = stream.Read(buf, BUFSIZE).LastRead();
		eof = (len < BUFSIZE);
		md5_update( &md5, buf, (int) len );
	} while (!eof);

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

int FbImportBook::FindBySize()
{
	wxArrayInt books;
	{
		wxString sql = wxT("SELECT DISTINCT id FROM books WHERE file_size=? AND (md5sum='' OR md5sum IS NULL)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, (wxLongLong)m_filesize);
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

void FbImportBook::AppendBook()
{
	Convert();

	int id_book = - m_database.NewId(DB_NEW_BOOK);
	long today = 0;
	wxDateTime::Now().Format(wxT("%y%m%d")).ToLong(&today);

	for (size_t i = 0; i<authors.Count(); i++) {
		{
			wxString sql = wxT("INSERT INTO books(id,id_archive,id_author,title,genres,file_name,file_path,file_size,file_type,lang,created,md5sum) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, m_archive);
			stmt.Bind(3, authors[i].id);
			stmt.Bind(4, title);
			stmt.Bind(5, genres);
			stmt.Bind(6, m_filename);
			stmt.Bind(7, m_filepath);
			stmt.Bind(8, (wxLongLong)m_filesize);
			stmt.Bind(9, wxFileName(m_filename).GetExt().Lower());
			stmt.Bind(10, lang);
			stmt.Bind(11, (int) today);
			stmt.Bind(12, m_md5sum);
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

	{
		wxString content = title;
		MakeLower(content);
		wxString sql = wxT("INSERT INTO fts_book(content, docid) VALUES(?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, content);
		stmt.Bind(2, id_book);
		stmt.ExecuteUpdate();
	}
}
void FbImportBook::AppendFile(int id_book)
{
	wxString sql = wxT("SELECT file_name FROM books WHERE id=? AND id_archive=? UNION SELECT file_name FROM files WHERE id_book=? AND id_archive=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id_book);
	stmt.Bind(2, m_archive);
	stmt.Bind(3, id_book);
	stmt.Bind(4, m_archive);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	while (result.NextRow()) {
		if (result.GetString(0) == m_filename) {
			wxLogWarning(_("File already exists %s"), m_message.c_str());
			return;
		}
	}

	wxLogWarning(_("Add alternative %s"), m_filename.c_str());
	{
		wxString sql = wxT("INSERT INTO files(id_book, id_archive, file_name, file_path) VALUES (?,?,?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id_book);
		stmt.Bind(2, m_archive);
		stmt.Bind(3, m_filename);
		stmt.Bind(4, m_filepath);
		stmt.ExecuteUpdate();
	}
}

void FbImportBook::Save()
{
	try {
		int id_book = FindByMD5();
		if (!id_book)
			id_book = FindBySize();
		if (id_book)
			AppendFile(id_book);
		else
			AppendBook();
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

//-----------------------------------------------------------------------------
//  FbImportZip
//-----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbZipEntryList);

FbImpotrZip::FbImpotrZip(FbImportThread *owner, wxInputStream &in, const wxString &zipname):
	m_database(owner->m_database),
	m_conv(wxT("cp866")),
	m_zip(in, m_conv),
	m_filename(owner->GetRelative(zipname)),
	m_filepath(owner->GetAbsolute(zipname)),
	m_filesize(in.GetLength()),
	m_ok(m_zip.IsOk())
{
	if (!m_ok) {
		wxLogError(wxT("Zip read error %s"), zipname.c_str());
		return;
	}
	wxLogInfo(_("Import zip %s"), m_filename.c_str());

    while (wxZipEntry * entry = m_zip.GetNextEntry()) {
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

wxZipEntry * FbImpotrZip::GetInfo(const wxString & filename)
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

int FbImpotrZip::Save()
{
	{
		wxString sql = wxT("SELECT id FROM archives WHERE file_name=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_filename);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		m_id = result.NextRow() ? result.GetInt(0) : 0;
	}

	wxString sql = m_id ?
		wxT("UPDATE archives SET file_name=?,file_path=?,file_size=?,file_count=? WHERE id=?") :
		wxT("INSERT INTO archives(file_name,file_path,file_size,file_count,id) VALUES (?,?,?,?,?)") ;

	if (!m_id) m_id = m_database.NewId(DB_NEW_ARCHIVE);

	{
		wxLongLong count = m_zip.GetTotalEntries();
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_filename);
		stmt.Bind(2, m_filepath);
		stmt.Bind(3, (wxLongLong)m_filesize);
		stmt.Bind(4, count);
		stmt.Bind(5, m_id);
		stmt.ExecuteUpdate();
	}

	return m_id;
}

void FbImpotrZip::Make(FbImportThread *owner)
{
	size_t skipped = 0;
	size_t existed = m_list.Count();
	if (owner) owner->DoStart(existed, m_filename);

	for (size_t i=0; i<existed; i++) {
		wxZipEntry * entry = m_list[i];
		if (owner) owner->DoStep(entry->GetInternalName());
		FbImportBook book(this, entry);
		if (book.IsOk()) book.Save(); else skipped++;
	}

	if ( existed && skipped ) wxLogWarning(wxT("FB2 and FBD not found %s"), m_filename.c_str());
	if ( !existed ) wxLogError(wxT("Zip read error %s"), m_filename.c_str());

	if (owner) owner->DoFinish();
}

//-----------------------------------------------------------------------------
//  FbImportThread
//-----------------------------------------------------------------------------

FbImportThread::FbImportThread():
	m_basepath(FbParams::GetText(DB_LIBRARY_DIR)),
	m_fullpath(FbParams::GetValue(FB_SAVE_FULLPATH))
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

wxString FbImportThread::GetAbsolute(const wxString &filename)
{
	return m_fullpath ? filename : (wxString)wxEmptyString;
}

//-----------------------------------------------------------------------------
//  FbZipImportThread
//-----------------------------------------------------------------------------

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

void FbZipImportThread::ImportFile(const wxString & zipname)
{
	wxLogInfo(_("Import file %s"), zipname.c_str());

	wxFFileInputStream in(zipname);
	if ( !in.IsOk() ) {
		wxLogError(wxT("File read error %s"), zipname.c_str());
		return;
	}

	DoStart(0, zipname);
	FbAutoCommit transaction(m_database);

	if (zipname.Right(4).Lower() == wxT(".fb2")) {
		FbImportBook book(this, in, zipname);
		if (book.IsOk()) book.Save();
		DoFinish();
	} else {
		FbImpotrZip zip(this, in, zipname);
		if (zip.IsOk()) {
			zip.Save();
			zip.Make(this);
		}
	}
}

//-----------------------------------------------------------------------------
//  FbDirImportThread
//-----------------------------------------------------------------------------

class CountTraverser : public wxDirTraverser
{
public:
	CountTraverser() : m_count(0) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		wxString ext = filename.Right(4).Lower();
		if (ext==wxT(".fb2") || ext==wxT(".zip")) m_count++;
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
			m_thread->ParseXml(filename);
		} else if (ext == wxT(".zip")) {
			Progress(filename);
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
	wxFFileInputStream in(filename);
	if ( in.IsOk() ) {
		FbAutoCommit transaction(m_database);
		FbImportBook book(this, in, filename);
		if (book.IsOk()) book.Save();
	} else {
		wxLogError(wxT("File read error %s"), filename.c_str());
	}
}

void FbDirImportThread::ParseZip(const wxString &zipname)
{
	wxFFileInputStream in(zipname);
	if ( in.IsOk() ){
		FbAutoCommit transaction(m_database);
		FbImpotrZip zip(this, in, zipname);
		if (zip.IsOk()) {
			zip.Save();
			zip.Make();
		}
	} else {
		wxLogError(wxT("File read error %s"), zipname.c_str());
	}
}
