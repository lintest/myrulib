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

//-----------------------------------------------------------------------------
//  FbImportBook
//-----------------------------------------------------------------------------

void FbImportBook::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (*this == wxT("/fictionbook/description/title-info")) {
		if (name == wxT("author")) {
			m_author = new AuthorItem;
			m_authors.Add(m_author);
		} else if (name == wxT("sequence")) {
			SequenceItem * seqitem = new SequenceItem(atts);
			m_sequences.Add(seqitem);
		}
	}
	m_text.Empty();
	Inc(name);
}

void FbImportBook::TxtNode(const wxString &text)
{
	if (Section() == fbsDescr) m_text << text;
}

void FbImportBook::EndNode(const wxString &name)
{
	Dec(name);
	if (*this == wxT("/fictionbook/description/title-info")) {
		m_text.Trim(false).Trim(true);
		if (name == wxT("book-title")) m_title = m_text; else
		if (name == wxT("genre")) m_genres += FbGenres::Char(m_text); else
		if (name == wxT("lang")) m_lang = m_text.Lower();
	} else if (*this == wxT("/fictionbook/description/title-info/author")) {
		m_text.Trim(false).Trim(true);
		if (name == wxT("last-name"))   m_author->last   = m_text; else
		if (name == wxT("first-name"))  m_author->first  = m_text; else 
		if (name == wxT("middle-name")) m_author->middle = m_text; 
	} else if (*this == wxT("/fictionbook/description/publish-info/")) {
		if (name == wxT("isbn")) m_isbn = m_text.Trim(true).Trim(false);
	} else if (*this == wxT("/fictionbook/description")) {
		if (name == wxT("title-info")) Stop();
	}
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
	m_ok = Parse(in);
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
	m_ok = Parse(owner->m_zip);
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
	for (size_t i = 0; i < m_authors.Count(); i++)
		m_authors[i].Convert(m_database);

	if (m_authors.Count() == 0)
		m_authors.Add(new AuthorItem);

	for (size_t i = 0; i < m_sequences.Count(); i++)
		m_sequences[i].Convert(m_database);

	if (m_sequences.Count() == 0)
		m_sequences.Add(new SequenceItem);
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

	for (size_t i = 0; i < m_authors.Count(); i++) {
		int author = m_authors[i].GetId();
		{
			wxString sql = wxT("INSERT INTO books(id,id_archive,id_author,title,genres,file_name,file_path,file_size,file_type,lang,created,md5sum) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, m_archive);
			stmt.Bind(3, author);
			stmt.Bind(4, m_title);
			stmt.Bind(5, m_genres);
			stmt.Bind(6, m_filename);
			stmt.Bind(7, m_filepath);
			stmt.Bind(8, (wxLongLong)m_filesize);
			stmt.Bind(9, wxFileName(m_filename).GetExt().Lower());
			stmt.Bind(10, m_lang);
			stmt.Bind(11, today);
			stmt.Bind(12, m_md5sum);
			ok = stmt.ExecuteUpdate() && ok;
		}
	}

    for (size_t i = 0; i < m_sequences.Count(); i++) {
		SequenceItem &sequence = m_sequences[i];
        wxString sql = wxT("INSERT INTO bookseq(id_book,id_seq,number) VALUES (?,?,?)");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, id_book);
        stmt.Bind(2, sequence.GetId());
        stmt.Bind(3, sequence.GetNumber());
		ok = stmt.ExecuteUpdate() && ok;
    }

	{
		wxString content = m_title;
		MakeLower(content);
		wxString sql = wxT("INSERT INTO fts_book(content,docid) VALUES(?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, content);
		stmt.Bind(2, id_book);
		ok = stmt.ExecuteUpdate() && ok;
	}

	if (m_database.TableExists(wxT("genres"))) {
		size_t count = m_genres.Length() / 2;
		for (size_t i = 0; i < count; i++) {
			wxString sql = wxT("INSERT INTO genres(id_book, id_genre) VALUES(?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, m_genres.Mid(i*2, 2));
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
