#include <wx/zipstrm.h>
#include <wx/dir.h>
#include "ImpThread.h"
#include "FbParams.h"
#include "FbManager.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "db/Sequences.h"
#include "MyRuLibMain.h"
#include "ZipReader.h"
#include "sha1/sha1.h"
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

	if (path == wxT("fictionbook/description/title-info/")) {
	    if (node_name == wxT("author")) {
            ctx->author = new AuthorItem;
	    } else if (node_name == wxT("sequence")) {
	        SequenceItem * seqitem = new SequenceItem;
            const XML_Char **a = atts;
            while (*a) {
                wxString attr = ctx->CharToLower(a[0]).Trim(false).Trim(true);
                wxString text = ctx->CharToString(a[1]).Trim(false).Trim(true);
                if (attr == wxT("name")) {
                    seqitem->seqname = text;
                } else if (attr == wxT("number")) {
                    text.ToLong(&seqitem->number);
                }
                a += 2;
            }
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
    ImportParsingContext *ctx = (ImportParsingContext*)userData;
    wxString node_name = ctx->CharToLower(name);
	ctx->RemoveTag(node_name);
    wxString path = ctx->Path();

	if (path == wxT("fictionbook/description/title-info/")) {
        ctx->text.Trim(false).Trim(true);
	    if (node_name == wxT("author")) {
	        ctx->authors.Add(ctx->author);
	    } else if (node_name == wxT("book-title")) {
	        ctx->title = ctx->text;
	    } else if (node_name == wxT("genre")) {
            ctx->genres += FbGenres::Char(ctx->text);
	    }
	} else if (path == wxT("fictionbook/description/title-info/author/")) {
        ctx->text.Trim(false).Trim(true);
        if (node_name == wxT("first-name"))
            ctx->author->first = ctx->text;
        if (node_name == wxT("middle-name"))
            ctx->author->middle = ctx->text;
        if (node_name == wxT("last-name"))
            ctx->author->last = ctx->text;
	} else if (path == wxT("fictionbook/description/")) {
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

ImportThread::ImportThread(wxEvtHandler *frame)
        : wxThread(), m_frame(frame)
{
}

void ImportThread::OnExit()
{
}

bool ImportThread::LoadXml(wxInputStream& stream, ImportParsingContext &ctx)
{
    const size_t BUFSIZE = 1024;
    unsigned char buf[BUFSIZE];
    bool done;

    sha1_context ctxSHA1;

    XML_SetUserData(ctx.GetParser(), (void*)&ctx);
    XML_SetElementHandler(ctx.GetParser(), StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(ctx.GetParser(), TextHnd);

    sha1_starts( &ctxSHA1 );

    bool ok = true;
    bool abort = false;

    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

		sha1_update( &ctxSHA1, buf, (int) len );

		if (!abort) {
			if ( !XML_Parse(ctx.GetParser(), (char *)buf, len, done) ) {
				XML_Error error_code = XML_GetErrorCode(ctx.GetParser());
				if ( error_code == XML_ERROR_ABORTED ) {
					abort = true;
				} else {
					wxString error(XML_ErrorString(error_code), *wxConvCurrent);
					wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(ctx.GetParser()));
					abort = true;
					ok = false;
					break;
				}
			}
        }
    } while (!done);

    unsigned char output[20];
    sha1_finish( &ctxSHA1, output );

	ctx.sha1sum = wxBase64Encode(output, 20).Left(27);

    for (size_t i=0; i<ctx.authors.Count(); i++) {
        ctx.authors[i].Convert();
    }
	if (ctx.authors.Count() == 0) ctx.authors.Add(new AuthorItem);

    for (size_t i=0; i<ctx.sequences.Count(); i++) {
        ctx.sequences[i].Convert();
    }

    return ok;
}

void ImportThread::AppendBook(ImportParsingContext &info, const wxString &name, const wxFileOffset size, int id_archive)
{
	long id_book = 0;
	{
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		id_book = - BookInfo::NewId(DB_NEW_BOOK);
	}

	for (size_t i = 0; i<info.authors.Count(); i++) {
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		Books books(wxGetApp().GetDatabase());
		Bookseq bookseq(wxGetApp().GetDatabase());

		BooksRow * row = books.New();
		row->id = id_book;
		row->id_author = info.authors[i].id;
		row->title = info.title;
		row->genres = info.genres;
		row->file_size = size;
		row->file_name = name;
		row->file_type = wxFileName(name).GetExt();
		row->id_archive = id_archive;
		row->sha1sum = info.sha1sum;
		row->Save();

		for (size_t j = 0; j<info.sequences.Count(); j++) {
			BookseqRow * seqRow = bookseq.New();
			seqRow->id_book = id_book;
			seqRow->id_seq = info.sequences[j].id;
			seqRow->id_author = info.authors[i].id;
			seqRow->number = info.sequences[j].number;
			seqRow->Save();
		}
	}
}

class AutoTransaction {
public:
    AutoTransaction()  { wxGetApp().GetDatabase()->BeginTransaction(); };
    ~AutoTransaction() { wxGetApp().GetDatabase()->Commit(); };
};

bool ImportThread::FindBySHA1(const wxString &sha1sum)
{
	wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT DISTINCT id FROM books WHERE sha1sum=?");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, sha1sum);
	DatabaseResultSet* result = pStatement->ExecuteQuery();

	return result && result->Next();
}

bool ImportThread::FindBySize(const wxString &sha1xml, wxFileOffset size)
{
	wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT DISTINCT id FROM books WHERE file_size=? AND (sha1sum='' OR sha1sum IS NULL)");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamInt(1, size);
	DatabaseResultSet* result = pStatement->ExecuteQuery();

	if (!result) return false;

	while (result->Next()) {
		int id = result->GetResultInt(wxT("id"));
		ZipReader book(id);
		if (!book.IsOK()) continue;

		ImportParsingContext info;
		LoadXml(book.GetZip(), info);
		wxString sha1book = info.sha1sum;

		wxString sql = wxT("UPDATE books SET sha1sum=? WHERE id=?");
		PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
		pStatement->SetParamString(1, sha1book);
		pStatement->SetParamInt(2, id);
		pStatement->ExecuteUpdate();
		if (sha1book == sha1xml) return true;
	}

	return false;
}

bool ImportThread::ParseXml(wxInputStream& stream, const wxString &name, int id_archive)
{
    ImportParsingContext info;

	if (LoadXml(stream, info)) {
		if (FindBySHA1(info.sha1sum)) return false;
		if (FindBySize(info.sha1sum, stream.GetLength())) return false;
        AppendBook(info, name, stream.GetLength(), id_archive);
        return true;
	}
    return false;
}

void ImportThread::PostEvent(wxEvent& event)
{
	wxPostEvent( m_frame, event );
}

int ImportThread::AddArchive(const wxString &filename)
{
    wxFileName file_name(filename);

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

ZipImportThread::ZipImportThread(wxEvtHandler *frame, const wxString &filename)
        :  ImportThread(frame), m_filename(filename)
{
}

void *ZipImportThread::Entry()
{
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

    AutoTransaction trans;

	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	int id_archive = AddArchive(m_filename);

	{
		wxFileName filename = m_filename;
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetInt(zip.GetTotalEntries());
		event.SetString(m_info + wxT(" ") + filename.GetFullName());
		PostEvent( event );
	}

	int progress = 0;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetName(wxPATH_UNIX);
			if (filename.Right(4).Lower() == wxT(".fb2")) {
				wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
				event.SetString(filename);
				event.SetInt(progress++);
                PostEvent( event );

				zip.OpenEntry(*entry);
                ParseXml(zip, filename, id_archive);
			}
		}
		delete entry;
	}

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
        PostEvent( event );
	}

	return NULL;
}

DirImportThread::DirImportThread(wxEvtHandler *frame, const wxString &dirname)
        :  ImportThread(frame), m_dirname(dirname)
{
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
    FolderTraverser(DirImportThread* thread) : m_thread(thread), m_progress(0) { }

    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
		wxString ext = filename.Right(4).Lower();

		if (ext == wxT(".fb2")) {
		    Progress(filename);
		    wxFFileInputStream file(filename);
            m_thread->ParseXml(file, filename, 0);
        } else if (ext == wxT(".zip")) {
		    Progress(filename);
            m_thread->ParseZip(filename);
        }

        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
        return wxDIR_CONTINUE;
    }
private:
    void Progress(const wxString &filename)
    {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
        event.SetString(wxFileName(filename).GetFullName());
        event.SetInt(m_progress++);
        m_thread->PostEvent( event );
    }
private:
    DirImportThread *m_thread;
    unsigned int m_progress;
};

void *DirImportThread::Entry()
{
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

    AutoTransaction trans;

    wxDir dir(m_dirname);
    if ( !dir.IsOpened() ) return NULL;

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetString(m_info + wxT(" ") + m_dirname);
		event.SetInt(1);
		PostEvent( event );

        CountTraverser counter;
        dir.Traverse(counter);

		event.SetInt(counter.GetCount());
		PostEvent( event );
	}

    FolderTraverser traverser(this);
    dir.Traverse(traverser);

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
        PostEvent( event );
	}

	return NULL;
}

bool DirImportThread::ParseZip(const wxString &filename)
{
	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	int id_archive = AddArchive(filename);

	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetName(wxPATH_UNIX);
			if (filename.Right(4).Lower() == wxT(".fb2")) {
				zip.OpenEntry(*entry);
                ParseXml(zip, filename, id_archive);
			}
		}
		delete entry;
	}

    return true;
}


