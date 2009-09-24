#include "ImpThread.h"
#include <wx/zipstrm.h>
#include <wx/dir.h>
#include "FbParams.h"
#include "FbManager.h"
#include "FbGenres.h"
#include "ZipReader.h"
#include "polarssl/md5.h"
#include "wx/base64.h"

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
    ImportParsingContext *ctx = (ImportParsingContext*)userData;
    wxString node_name = ctx->CharToLower(name);
    wxString path = ctx->Path();

	if (path == wxT("/fictionbook/description/title-info")) {
	    if (node_name == wxT("author")) {
            ctx->author = new AuthorItem;
	        ctx->authors.Add(ctx->author);
	    } else if (node_name == wxT("sequence")) {
	        SequenceItem * seqitem = new SequenceItem;
            ctx->sequences.Add(seqitem);
            const XML_Char **a = atts;
            while (*a) {
                wxString attr = ctx->CharToLower(a[0]).Trim(false).Trim(true); a++;
                wxString text = ctx->CharToString(a[0]).Trim(false).Trim(true); a++;
                if (attr == wxT("name")) {
                    seqitem->seqname = text;
                } else if (attr == wxT("number")) {
                    text.ToLong(&seqitem->number);
                }
            }
	    }
	}
	ctx->AppendTag(node_name);
    ctx->text.Empty();
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
    ImportParsingContext *ctx = (ImportParsingContext*)userData;
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
    ImportParsingContext *ctx = (ImportParsingContext*)userData;

    wxString str = ctx->CharToString(s, len);

    if (!ctx->IsWhiteOnly(str)) ctx->text += str;
}
}

wxSQLite3Statement ImportThread::GetPreparedStatement(PSItem psItem)
{
    return m_database.PrepareStatement(GetSQL(psItem));
}

void ImportThread::OnExit()
{
}

wxString ImportThread::GetSQL(PSItem psItem)
{
    switch (psItem) {
        case psFindBySize: return wxT("SELECT DISTINCT id FROM books WHERE file_size=? AND (md5sum='' OR md5sum IS NULL)");
        case psFindByMd5: return wxT("SELECT id FROM books WHERE md5sum=?");
        case psUpdateMd5: return wxT("UPDATE books SET md5sum=? WHERE id=?");
        case psSearchFile: return wxT("SELECT file_name, file_path FROM books WHERE id=? AND id_archive=? UNION SELECT file_name, file_path FROM files WHERE id_book=? AND id_archive=?");
        case psAppendFile: return wxT("INSERT INTO files(id_book, id_archive, file_name, file_path) VALUES (?,?,?,?)");
        case psSearchArch: return wxT("SELECT id FROM archives WHERE file_name=? AND file_path=?");
        case psAppendArch: return wxT("INSERT INTO archives(id, file_name, file_path, file_size, file_count) VALUES (?,?,?,?,?)");
        case psAppendBook: return wxT("INSERT INTO books(id, id_archive, id_author, title, genres, file_name, file_path, file_size, file_type, md5sum) VALUES (?,?,?,?,?,?,?,?,?,?)");
        case psAppendSeqs: return wxT("INSERT INTO bookseq(id_book, id_seq, number, id_author) VALUES (?,?,?,?)");
        case psLastMember: return wxEmptyString;
    }
    return wxEmptyString;
}

bool ImportThread::LoadXml(wxInputStream& stream, ImportParsingContext &ctx)
{
    const size_t BUFSIZE = 1024;
    unsigned char buf[BUFSIZE];
    bool done;

    md5_context md5;

    XML_SetElementHandler(ctx.GetParser(), StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(ctx.GetParser(), TextHnd);

    md5_starts( &md5 );

    bool ok = true;
    bool skip = ctx.md5only;

    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

		md5_update( &md5, buf, (int) len );

		if (!skip) {
			if ( !XML_Parse(ctx.GetParser(), (char *)buf, len, done) ) {
				XML_Error error_code = XML_GetErrorCode(ctx.GetParser());
				if ( error_code == XML_ERROR_ABORTED ) {
					skip = true;
				} else {
					wxString error(XML_ErrorString(error_code), *wxConvCurrent);
					wxLogError(_("XML parsing error: '%s' at line %d file %s"), error.c_str(), XML_GetCurrentLineNumber(ctx.GetParser()), ctx.filename.c_str());
					skip = true;
					ok = false;
					break;
				}
			}
        }
    } while (!done);

    unsigned char output[16];
    md5_finish( &md5, output );
    ctx.md5sum = wxEmptyString;
    for (size_t i=0; i<16; i++) ctx.md5sum += wxString::Format(wxT("%02x"), output[i]);
    memset( &md5, 0, sizeof( md5_context ) );

    for (size_t i=0; i<ctx.authors.Count(); i++)
		ctx.authors[i].Convert(m_database);

	if (ctx.authors.Count() == 0)
		ctx.authors.Add(new AuthorItem);

    for (size_t i=0; i<ctx.sequences.Count(); i++)
        ctx.sequences[i].Convert(m_database);

    return ok;
}

void ImportThread::AppendBook(ImportParsingContext &info, const wxString &name, const wxString &path, const wxFileOffset size, int id_archive)
{
	int id_book = - m_database.NewId(DB_NEW_BOOK);

	for (size_t i = 0; i<info.authors.Count(); i++) {
        wxSQLite3Statement stmt = GetPreparedStatement(psAppendBook);

        stmt.Bind(1, id_book);
        stmt.Bind(2, id_archive);
        stmt.Bind(3, info.authors[i].id);
        stmt.Bind(4, info.title);
        stmt.Bind(5, info.genres);
        stmt.Bind(6, name);
        stmt.Bind(7, path);
        stmt.Bind(8, (wxLongLong)size);
        stmt.Bind(9, wxFileName(name).GetExt().Lower());
        stmt.Bind(10, info.md5sum);
        stmt.ExecuteUpdate();

		for (size_t j = 0; j<info.sequences.Count(); j++) {
            wxSQLite3Statement stmt = GetPreparedStatement(psAppendSeqs);
            stmt.Bind(1, id_book);
            stmt.Bind(2, info.sequences[j].id);
            stmt.Bind(3, (int)info.sequences[j].number);
            stmt.Bind(4, info.authors[i].id);
            stmt.ExecuteUpdate();
		}
	}
}

int ImportThread::FindByMD5(const wxString &md5sum)
{
    wxSQLite3Statement stmt = GetPreparedStatement(psFindByMd5);
	stmt.Bind(1, md5sum);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0) : 0;
}

int ImportThread::FindBySize(const wxString &md5sum, wxFileOffset size)
{
    wxArrayInt books;

    {
        wxSQLite3Statement stmt = GetPreparedStatement(psFindBySize);
        stmt.Bind(1, (wxLongLong)size);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        while (result.NextRow()) {
            books.Add(result.GetInt(0));
        }
    }

    for (size_t i=0; i<books.Count(); i++) {
		ZipReader book(books[i]);
		if (!book.IsOK()) continue;

		ImportParsingContext info;
		info.md5only = true;
		LoadXml(book.GetZip(), info);

        wxSQLite3Statement stmt = GetPreparedStatement(psUpdateMd5);
        stmt.Bind(1, info.md5sum);
        stmt.Bind(2, books[i]);
		stmt.ExecuteUpdate();

		if (info.md5sum == md5sum) return books[i];
	}

	return 0;
}

bool ImportThread::ParseXml(wxInputStream& stream, const wxString &name, const wxString &path, const int id_archive)
{
    ImportParsingContext info;

    info.filename = name;
    info.filepath = path;

    try {
        if (LoadXml(stream, info)) {
            int id_book = FindByMD5(info.md5sum);
            if (id_book == 0)
                id_book = FindBySize(info.md5sum, stream.GetLength());
            if (id_book == 0)
                AppendBook(info, name, path, stream.GetLength(), id_archive);
            else
                AppendFile(id_book, id_archive, name, path);
            return true;
        }
    } catch (wxSQLite3Exception & e) {
        wxLogFatalError(e.GetMessage());
    }
    return false;
}

void ImportThread::AppendFile(const int id_book, const int id_archive, const wxString &new_name, const wxString &new_path)
{
    wxSQLite3Statement stmt = GetPreparedStatement(psSearchFile);
    stmt.Bind(1, id_book);
    stmt.Bind(2, id_archive);
    stmt.Bind(3, id_book);
    stmt.Bind(4, id_archive);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

    wxString old_name, old_path;
    if (result.NextRow()) {
        old_name = result.GetString(0);
        old_path = result.GetString(1);
    }

    if (old_name == new_name && old_path == new_path) {
        wxLogWarning(_("File already exists %s %s"), new_path.c_str(), new_name.c_str());
        return;
    }

    wxLogWarning(_("Add alternative %s %s"), new_path.c_str(), new_name.c_str());
    stmt = GetPreparedStatement(psAppendFile);
    stmt.Bind(1, id_book);
    stmt.Bind(2, id_archive);
    stmt.Bind(3, new_name);
    stmt.Bind(4, new_path);
    stmt.ExecuteUpdate();
}

int ImportThread::AddArchive(const wxString &name, const wxString &path, const int size, const int count)
{
    {
        wxSQLite3Statement stmt = GetPreparedStatement(psSearchArch);
        stmt.Bind(1, name);
        stmt.Bind(2, path);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) return result.GetInt(0);
    }

    int id = m_database.NewId(DB_NEW_ARCHIVE);
    {
        wxSQLite3Statement stmt = GetPreparedStatement(psAppendArch);
        stmt.Bind(1, id);
        stmt.Bind(2, name);
        stmt.Bind(3, path);
        stmt.Bind(4, size);
        stmt.Bind(5, (wxLongLong)count);
        stmt.ExecuteUpdate();
    }
	return id;
}

void *ZipImportThread::Entry()
{
    size_t count = m_filelist.Count();
    wxLogInfo(_("Start import %d file(s)"), count);
    for (size_t i=0; i<count; i++) {
        ImportFile(m_filelist[i]);
    }
    UpdateBooksCount();
    wxLogInfo(_("Finish import %d file(s)"), count);
    return NULL;
}

void ZipImportThread::ImportFile(const wxString & zipname)
{
    wxCriticalSectionLocker enter(sm_queue);

    wxLogInfo(_("Import file %s"), zipname.c_str());

    FbAutoCommit transaction(GetDatabase());

	wxFFileInputStream in(zipname);
	if ( !in.IsOk() ){
	    wxLogError(wxT("File read error %s"), zipname.c_str());
	    return;
	}

	if (zipname.Right(4).Lower() == wxT(".fb2")) {
        DoStart(0, zipname);
        wxFileName filename = zipname;
        filename.Normalize(wxPATH_NORM_ALL);
        ParseXml(in, filename.GetFullName(), filename.GetPath(wxPATH_UNIX), 0);
        DoFinish();
        return;
	}

    wxCSConv conv(wxT("cp866"));
	wxZipInputStream zip(in, conv);
	if ( !zip.IsOk() ){
	    wxLogError(wxT("Zip read error %s"), zipname.c_str());
	    return;
	}

    wxFileName filename = zipname;
    filename.Normalize(wxPATH_NORM_ALL);
	int id_archive = AddArchive(filename.GetFullName(), filename.GetPath(wxPATH_UNIX), in.GetLength(), zip.GetTotalEntries());

    DoStart(zip.GetTotalEntries(), zipname);

    bool ok = false;
    bool skip = true;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
		    ok = true;
			wxString filename = entry->GetInternalName();
			if (filename.Right(4).Lower() == wxT(".fb2")) {
			    skip = false;
                wxLogInfo(_("Import zip entry %s"), filename.c_str());
			    DoStep(filename);
				zip.OpenEntry(*entry);
                ParseXml(zip, filename, wxEmptyString, id_archive);
			} else {
                wxLogWarning(_("Skip file %s"), filename.c_str());
			}
		}
		delete entry;
	}

	if ( ok && skip ) wxLogWarning(wxT("Fb2 not found %s"), zipname.c_str());
	if ( !ok ) wxLogError(wxT("Zip read error %s"), zipname.c_str());

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
    FolderTraverser(DirImportThread* thread) : m_thread(thread), m_progress(0) { };

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
    DirImportThread *m_thread;
    unsigned int m_progress;
};

wxString DirImportThread::Normalize(const wxString &filename)
{
    wxFileName result = filename;
    result.Normalize(wxPATH_NORM_ALL);
    return result.GetFullPath();
}

wxString DirImportThread::Relative(const wxString &filename)
{
    wxFileName result = filename;
    result.Normalize(wxPATH_NORM_ALL);
    return result.GetFullPath().Mid(m_position);
}

DirImportThread::DirImportThread(const wxString &dirname)
    : m_dirname(Normalize(dirname)), m_position(m_dirname.Length() + (m_dirname.IsEmpty() ? 0 : 1) )
{
}

void *DirImportThread::Entry()
{
    wxCriticalSectionLocker enter(sm_queue);

    FbAutoCommit transaction(GetDatabase());

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

    UpdateBooksCount();
    DoFinish();

    wxLogInfo(_("Finish import directory %s"), m_dirname.c_str());

	return NULL;
}

bool DirImportThread::ParseZip(const wxString &zipname)
{
	wxFFileInputStream in(zipname);
	if ( !in.IsOk() ){
	    wxLogError(wxT("Zip read error %s"), zipname.c_str());
	    return false;
	}

    wxCSConv conv(wxT("cp866"));
	wxZipInputStream zip(in, conv);
	if ( !zip.IsOk() ){
	    wxLogError(wxT("Zip read error %s"), zipname.c_str());
	    return false;
	}

	int id_archive = AddArchive(Relative(zipname), m_dirname, in.GetLength(), zip.GetTotalEntries());

    bool ok = false;
    bool skip = true;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
            ok = true;
			wxString filename = entry->GetInternalName();
			if (filename.Right(4).Lower() == wxT(".fb2")) {
			    skip = false;
                wxLogInfo(_("Import zip entry %s"), filename.c_str());
				zip.OpenEntry(*entry);
                ImportThread::ParseXml(zip, filename, wxEmptyString, id_archive);
			} else {
                wxLogWarning(_("Skip file %s"), filename.c_str());
			}
		}
		delete entry;
	}

	if ( ok && skip ) wxLogWarning(wxT("Fb2 not found %s"), zipname.c_str());
	if ( !ok ) wxLogError(wxT("Zip read error %s"), zipname.c_str());

    return ok;
}

bool DirImportThread::ParseXml(const wxString &filename)
{
    wxFFileInputStream file(filename);
    return ImportThread::ParseXml(file, Relative(filename), m_dirname, 0);
}

void BooksCountThread::Execute()
{
    BooksCountThread * thread = new BooksCountThread();
    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }
    thread->Run();
}

void * BooksCountThread::Entry()
{
    wxCriticalSectionLocker enter(sm_queue);
    UpdateBooksCount();
    return NULL;
}
