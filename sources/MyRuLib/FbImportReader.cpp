#include "FbImportReader.h"
#include "FbImportThread.h"
#include <wx/dir.h>
#include <wx/list.h>
#include "FbConst.h"
#include "FbDateTime.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "MyRuLibApp.h"
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

void FbImportBook::NewNode(const wxString &name, const FbStringHash &atts)
{
	/*
	Inc(name);
	switch (Section()) {
		case fbsDescr: {
			if (*this > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description/title-info/coverpage/image")) {
				AppendImg(atts);
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn.Empty();
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << wxString::Format(wxT("<%s>"), name.c_str());
		} break;
		case fbsBinary: {
			StartImg(atts);
		} break;
		case fbsNone: {
		} break;
	}
	*/
}

void FbImportBook::TxtNode(const wxString &text)
{
	/*
	switch (Section()) {
		case fbsDescr: {
			if (*this >= wxT("fictionbook/description/title-info/annotation")) {
				m_annt << text;
			} else if (*this == wxT("fictionbook/description/publish-info/isbn")) {
				m_isbn << text;
			}
		} break;
		case fbsBody: {
			if (m_parsebody) m_annt << text;
		} break;
		case fbsBinary: {
			if (m_saveimage) m_imagedata << text;
		} break;
		case fbsNone: {
		} break;
	}
	*/
}

void FbImportBook::EndNode(const wxString &name)
{
	/*
	switch (Section()) {
		case fbsDescr: {
			if (*this > wxT("fictionbook/description/title-info/annotation")) {
				m_annt << wxString::Format(wxT("</%s>"), name.c_str());
			} else if (*this == wxT("fictionbook/description")) {
				m_parsebody = m_annt.IsEmpty();
				if (!m_parsebody) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count() == 0) Stop();
				}
				m_data.SetText(FbViewData::ISBN, m_isbn);
				m_thread.SendHTML(m_data);
			}
		} break;
		case fbsBody: {
			if (m_parsebody) {
				m_annt << wxString::Format(wxT("<%s>"), name.c_str());
				if (m_annt.Length() > 1000) {
					m_data.SetText(FbViewData::ANNT, m_annt);
					if (m_images.Count()==0) Stop();
					m_thread.SendHTML(m_data);
					m_parsebody = false;
				}
			}
		} break;
		case fbsBinary: {
			if (m_saveimage) {
				m_data.AddImage(m_imagename, m_imagedata);
				m_thread.SendHTML(m_data);
			}
		} break;
		case fbsNone: {
		} break;
	}
	Dec(name);
	*/
}

FbImportBook::FbImportBook(FbImportThread *owner, wxInputStream &in, const wxString &filename):
	m_database(owner->m_database),
	m_filename(owner->GetRelative(filename)),
	m_filepath(owner->GetAbsolute(filename)),
	m_message(filename),
	m_filesize(in.GetLength()),
	m_archive(0),
	m_ok(false)
{
	wxLogMessage(_("Import file %s"), m_filename.c_str());
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
	wxLogMessage(_("Import zip entry %s"), m_filename.c_str());
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

	if (sequences.Count() == 0)
		sequences.Add(new SequenceItem);
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
		if (!book.IsOk()) continue;
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

bool FbImportBook::AppendBook()
{
	bool ok = true;

	Convert();

	int id_book = - m_database.NewId(DB_NEW_BOOK);
	int today = FbDateTime::Today().Code();

	for (size_t i = 0; i<authors.Count(); i++) {
		int author = authors[i].GetId();
		{
			wxString sql = wxT("INSERT INTO books(id,id_archive,id_author,title,genres,file_name,file_path,file_size,file_type,lang,created,md5sum) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, m_archive);
			stmt.Bind(3, author);
			stmt.Bind(4, title);
			stmt.Bind(5, genres);
			stmt.Bind(6, m_filename);
			stmt.Bind(7, m_filepath);
			stmt.Bind(8, (wxLongLong)m_filesize);
			stmt.Bind(9, wxFileName(m_filename).GetExt().Lower());
			stmt.Bind(10, lang);
			stmt.Bind(11, today);
			stmt.Bind(12, m_md5sum);
			ok = stmt.ExecuteUpdate() && ok;
		}
	}

    for (size_t j = 0; j<sequences.Count(); j++) {
		SequenceItem &sequence = sequences[j];
        wxString sql = wxT("INSERT INTO bookseq(id_book,id_seq,number) VALUES (?,?,?)");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, id_book);
        stmt.Bind(2, sequence.GetId());
        stmt.Bind(3, sequence.GetNumber());
		ok = stmt.ExecuteUpdate() && ok;
    }

	{
		wxString content = title;
		MakeLower(content);
		wxString sql = wxT("INSERT INTO fts_book(content,docid) VALUES(?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, content);
		stmt.Bind(2, id_book);
		ok = stmt.ExecuteUpdate() && ok;
	}

	if (m_database.TableExists(wxT("genres"))) {
		size_t count = genres.Length() / 2;
		for (size_t i = 0; i < count; i++) {
			wxString sql = wxT("INSERT INTO genres(id_book, id_genre) VALUES(?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, genres.Mid(i*2, 2));
			ok = stmt.ExecuteUpdate() && ok;
		}
	}

	FbCounter::AddBook(m_database, id_book);

	return ok;
}
bool FbImportBook::AppendFile(int id_book)
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
			return true;
		}
	}

	wxLogWarning(_("Add alternative %s"), m_filename.c_str());
	sql = wxT("INSERT INTO files(id_book, id_archive, file_name, file_path) VALUES (?,?,?,?)");
	stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id_book);
	stmt.Bind(2, m_archive);
	stmt.Bind(3, m_filename);
	stmt.Bind(4, m_filepath);
	return stmt.ExecuteUpdate();
}

bool FbImportBook::Save()
{
	FbAutoCommit transaction(m_database);
	int id_book = FindByMD5();
	if (!id_book) id_book = FindBySize();
	return id_book ? AppendFile(id_book) : AppendBook();
}

