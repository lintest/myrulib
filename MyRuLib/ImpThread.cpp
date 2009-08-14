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

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;
extern wxString strParsingInfo;

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

ParseThread::ParseThread(wxEvtHandler *frame)
        : wxThread(), m_frame(frame)
{
}

void ParseThread::OnExit()
{
}

bool ParseThread::LoadXml(wxInputStream& stream, ImportParsingContext &ctx)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    bool done;

    XML_SetUserData(ctx.GetParser(), (void*)&ctx);
    XML_SetElementHandler(ctx.GetParser(), StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(ctx.GetParser(), TextHnd);

    bool ok = true;
    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

        if ( !XML_Parse(ctx.GetParser(), buf, len, done) ) {
			XML_Error error_code = XML_GetErrorCode(ctx.GetParser());
			if ( error_code == XML_ERROR_ABORTED ) {
				done = true;
			} else {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(ctx.GetParser()));
				ok = false;
	            break;
			}
        }
    } while (!done);

    for (size_t i=0; i<ctx.authors.Count(); i++) {
        ctx.authors[i].Convert();
    }
	if (ctx.authors.Count() == 0) ctx.authors.Add(new AuthorItem);

    for (size_t i=0; i<ctx.sequences.Count(); i++) {
        ctx.sequences[i].Convert();
    }

    return ok;
}

void ParseThread::AppendBook(ImportParsingContext &info, const wxString &name, const wxFileOffset size, int id_archive)
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

bool ParseThread::ParseXml(wxInputStream& stream, const wxString &name, const wxFileOffset size, int id_archive)
{
    ImportParsingContext info;

	if (LoadXml(stream, info)) {
        AppendBook(info, name, size, id_archive);
        return true;
	}
    return false;
}

void ParseThread::PostEvent(wxEvent& event)
{
	wxPostEvent( m_frame, event );
}

ImportThread::ImportThread(wxEvtHandler *frame, const wxString &filename)
        :  ParseThread(frame), m_filename(filename)
{
}

int ParseThread::AddArchive(const wxString &filename)
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

void *ImportThread::Entry()
{
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	int id_archive = AddArchive(m_filename);

	{
		wxFileName filename = m_filename;
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetInt(zip.GetTotalEntries());
		event.SetString(strParsingInfo + filename.GetFullName());
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
                ParseXml(zip, filename, entry->GetSize(), id_archive);
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

FolderThread::FolderThread(wxEvtHandler *frame, const wxString &dirname)
        :  ParseThread(frame), m_dirname(dirname)
{
}

class CountTraverser : public wxDirTraverser
{
public:
    CountTraverser() : m_count(0) { }
    virtual wxDirTraverseResult OnFile(const wxString& filename) {
        m_count++;
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
    FolderTraverser(FolderThread* thread) : m_thread(thread), m_progress(0) { }

    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
        event.SetString(wxFileName(filename).GetFullName());
        event.SetInt(m_progress++);
        m_thread->PostEvent( event );

        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
        return wxDIR_CONTINUE;
    }

private:
    FolderThread *m_thread;
    unsigned int m_progress;
};

void *FolderThread::Entry()
{
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

    wxDir dir(m_dirname);
    if ( !dir.IsOpened() ) return NULL;

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetString(strParsingInfo + m_dirname);
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
