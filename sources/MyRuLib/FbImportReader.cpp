#include "FbImportReader.h"
#include "FbImportThread.h"
#include <wx/dir.h>
#include <wx/list.h>
#include "FbConst.h"
#include "FbDateTime.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "FbSmartPtr.h"
#include "MyRuLibApp.h"
#include "polarssl/md5.h"
#include "wx/base64.h"

wxString Ext(const wxString &filename)
{
	return filename.AfterLast(wxT('.')).Lower();
}

wxString Md5(md5_context & md5)
{
	wxString md5sum;
	unsigned char output[16];
	md5_finish( &md5, output );
	for (size_t i = 0; i < 16; i++)
		md5sum << wxString::Format(wxT("%02x"), output[i]);
	return md5sum;
}

//-----------------------------------------------------------------------------
//  FbImportZip
//-----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbZipEntryList);

int FbImportZip::Exists(FbDatabase & database, const wxString & filename)
{
	return database.Int(filename, wxT("SELECT id FROM archives WHERE file_name=?"));
}

FbImportZip::FbImportZip(FbImportThread & owner, wxInputStream &in, const wxString &filename):
	m_owner(owner),
	m_database(*owner.GetDatabase()),
	m_conv(wxT("cp866")),
	m_zip(in, m_conv),
	m_filename(owner.GetRelative(filename)),
	m_filepath(owner.GetAbsolute(filename)),
	m_filesize(in.GetLength()),
	m_ok(m_zip.IsOk())
{
	if (!m_ok) {
		wxLogError(_("Zip read error %s"), filename.c_str());
		return;
	}
	wxLogMessage(_("Import zip %s"), filename.c_str());

	while (wxZipEntry * entry = m_zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetInternalName();
			if (Ext(filename) == wxT("fbd")) {
				wxString infoname = filename.BeforeLast(wxT('.'));
				wxZipEntry*& current = m_map[infoname];
				delete current;
				current = entry;
			} else {
				m_list.Add(entry);
			}
		}
		if (m_owner.IsClosed()) break;
	}
}

wxZipEntry * FbImportZip::GetInfo(const wxString & filename)
{
	wxString infoname = filename.BeforeLast(wxT('.'));
	FbZipEntryMap::iterator it = m_map.find(infoname);
	return (it == m_map.end()) ? NULL : it->second;
}

int FbImportZip::Save(bool progress)
{
	m_id = m_database.Int(m_filename, wxT("SELECT id FROM archives WHERE file_name=?"));

	wxString sql = m_id ?
		wxT("UPDATE archives SET file_name=?,file_path=?,file_size=?,file_count=? WHERE id=?") :
		wxT("INSERT INTO archives(file_name,file_path,file_size,file_count,id) VALUES (?,?,?,?,?)") ;

	if (!m_id) m_id = - m_database.NewId(DB_NEW_ARCHIVE);

	{
		wxLongLong count = m_zip.GetTotalEntries();
		FbSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_filename);
		stmt.Bind(2, m_filepath);
		stmt.Bind(3, (wxLongLong)m_filesize);
		stmt.Bind(4, count);
		stmt.Bind(5, m_id);
		stmt.ExecuteUpdate();
	}

	Make(progress);

	return m_id;
}

void FbImportZip::Make(bool progress)
{
	wxString info = _("Processing file:"); info << wxT(' ');
	size_t skipped = 0;
	size_t existed = m_list.Count();
	if (progress) m_owner.DoStart(info + m_filename, existed);

	size_t processed = 0;
	for (size_t i=0; i<existed; i++) {
		wxZipEntry * entry = m_list[i];
		if (progress) m_owner.DoStep(entry->GetInternalName());
		bool ok = FbImportBook(*this, *entry).Save();
		if (ok) processed++; else skipped++;
	}

	if ( existed && processed == 0 ) {
		wxLogWarning(_("FB2 and FBD not found %s"), m_filename.c_str());
	}

	if ( existed == 0 ) {
		wxLogError(_("Zip read error %s"), m_filename.c_str());
	}

	if (progress) m_owner.DoFinish();
}

//-----------------------------------------------------------------------------
//  FbImportReader
//-----------------------------------------------------------------------------

static int CompareAuthors(AuthorItem ** n1, AuthorItem ** n2)
{
	return (*n1)->GetId() - (*n2)->GetId();
}

void FbImportReader::Convert(FbDatabase & database)
{
	m_title = m_title.Trim(false).Trim(true);
	m_lang = Lower(m_lang).Trim(false).Trim(true);
	m_dscr = m_dscr.Trim(false).Trim(true);
	m_isbn = m_isbn.Trim(false).Trim(true);

	for (size_t i = 0; i < m_authors.Count(); i++)
		m_authors[i].Convert(database);

	if (m_authors.Count()) {
		m_authors.Sort(CompareAuthors);
	} else {
		m_authors.Add(new AuthorItem);
	}

	for (size_t i = 0; i < m_sequences.Count(); i++)
		m_sequences[i].Convert(database);

	if (m_sequences.Count() == 0)
		m_sequences.Add(new SequenceItem);
}

void FbImportReader::OnError(wxLogLevel level, const wxString &msg, int line)
{
	wxLogError(_("XML parsing error: '%s' at line %d"), msg.c_str(), line);
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::RootHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbImportReaderFB2::RootHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	return name == wxT("description") ? new DscrHandler(m_reader, name) : NULL;
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::DscrHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbImportReaderFB2::DscrHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	return name == wxT("title-info") ? new TitleHandler(m_reader, name) : NULL;
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::TitleHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbImportReaderFB2::TitleHandler)
	KEY( "book-title"   , Title    );
	KEY( "author"       , Author   );
	KEY( "annotation"   , Annot    );
	KEY( "sequence"     , Sequence );
	KEY( "genre"        , Genre    );
	KEY( "lang"         , Lang     );
FB2_END_KEYHASH

FbHandlerXML * FbImportReaderFB2::TitleHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Author   : return new AuthorHandler(m_reader, name);
		case Title    : return new TextHandler(name, m_reader.m_title);
		case Annot    : return new AnnotHandler(m_reader, name);
		case Sequence : return new SeqnHandler(m_reader, name, atts);
		case Genre    : return new GenrHandler(m_reader, name);
		case Lang     : return new TextHandler(name, m_reader.m_lang);
		default       : return NULL;
	}
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::AuthorHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbImportReaderFB2::AuthorHandler)
	KEY( "last-name"    , Last   );
	KEY( "first-name"   , First  );
	KEY( "middle-name"  , Middle );
FB2_END_KEYHASH

FbImportReaderFB2::AuthorHandler::AuthorHandler(FbImportReader &reader, const wxString &name)
	: FbHandlerXML(name), m_author(new AuthorItem)
{
	reader.m_authors.Add(m_author);
}

FbHandlerXML * FbImportReaderFB2::AuthorHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Last   : return new TextHandler(name, m_author->last);
		case First  : return new TextHandler(name, m_author->first);
		case Middle : return new TextHandler(name, m_author->middle);
		default     : return NULL;
	}
	return FbHandlerXML::NewNode(name, atts);
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::AnnotHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbImportReaderFB2::AnnotHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (name == wxT('p')) m_reader.m_dscr << wxT('<') << name << wxT('>');
	return new AnnotHandler(*this, name);
}

void FbImportReaderFB2::AnnotHandler::TxtNode(const wxString &text)
{
	m_reader.m_dscr << text;
}

void FbImportReaderFB2::AnnotHandler::EndNode(const wxString &name)
{
	if (name == wxT('p')) m_reader.m_dscr << wxT('<') << wxT('/') << name << wxT('>');
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::SeqnHandler
//-----------------------------------------------------------------------------

FbImportReaderFB2::SeqnHandler::SeqnHandler(FbImportReader &reader, const wxString &name, const FbStringHash &atts)
	: FbHandlerXML(name)
	, m_reader(reader)
{
	reader.m_sequences.Add(new SequenceItem(atts));
}

FbHandlerXML * FbImportReaderFB2::SeqnHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	return name == wxT("sequence") ? new SeqnHandler(m_reader, name, atts) : NULL;
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2::GenrHandler
//-----------------------------------------------------------------------------

void FbImportReaderFB2::GenrHandler::EndNode(const wxString &name)
{
	m_reader.m_genres += FbGenres::Char(m_text);
}

//-----------------------------------------------------------------------------
//  FbImportReaderFB2
//-----------------------------------------------------------------------------

FbImportReaderFB2::FbImportReaderFB2(wxInputStream & stream, bool md5)
{
	m_ok = Parse(stream, md5);
}

FbHandlerXML * FbImportReaderFB2::CreateHandler(const wxString &name)
{
	return name == wxT("fictionbook") ? new RootHandler(*this, name) : NULL;
}

//-----------------------------------------------------------------------------
//  FbImportBook
//-----------------------------------------------------------------------------

int FbImportBook::Exists(FbDatabase & database, const wxString & filename)
{
	return database.Int(filename, wxT("SELECT id FROM books WHERE file_name=?1 AND id_archive=0 UNION SELECT id_book FROM files WHERE file_name=?1 AND id_archive=0"));
}

FbImportBook::FbImportBook(FbImportThread & owner, wxInputStream & in, const wxString & filename)
	: m_parser(NULL)
	, m_database(*owner.GetDatabase())
	, m_filename(owner.GetRelative(filename))
	, m_filepath(owner.GetAbsolute(filename))
	, m_filetype(Ext(m_filename))
	, m_message(filename)
	, m_filesize(in.GetLength())
	, m_archive(0)
	, m_ok(false)
{
	wxLogMessage(_("Import file %s"), m_filename.c_str());
	m_ok = in.IsOk();
	if (!m_ok) return;

	if (m_filetype == wxT("fb2")) {
		m_parser = new FbImportReaderFB2(in, true);
		m_md5sum = m_parser->GetMd5();
	} else if (m_filetype == wxT("epub")) {
		m_md5sum = CalcMd5(in);
		in.SeekI(0);
		wxString rootfile = FbRootReaderEPUB(in).GetRoot();
		in.SeekI(0);
		m_parser = new FbDataReaderEPUB(in, rootfile);
	} else {
		m_md5sum = CalcMd5(in);
	}
}

FbImportBook::FbImportBook(FbImportZip & owner, wxZipEntry & entry)
	: m_parser(NULL)
	, m_database(owner.GetDatabase())
	, m_filename(entry.GetInternalName())
	, m_filetype(Ext(m_filename))
	, m_message(owner.GetFilename() + wxT(": ") + m_filename)
	, m_filesize(entry.GetSize())
	, m_archive(owner.GetId())
	, m_ok(false)
{
	if (m_filetype == wxT("fbd")) return;

	wxLogMessage(_("Import zip entry %s"), m_filename.c_str());
	if (!(m_ok = owner.OpenEntry(entry))) return;

	if (m_filetype == wxT("fb2")) {
		m_parser = new FbImportReaderFB2(owner, m_md5sum.IsEmpty());
		m_md5sum = m_parser->GetMd5();
	} else if (m_filetype == wxT("epub")) {
		m_md5sum = CalcMd5(owner);
		if (!owner.OpenEntry(entry)) return;
		wxString rootfile = FbRootReaderEPUB(owner).GetRoot();
		if (!owner.OpenEntry(entry)) return;
		m_parser = new FbDataReaderEPUB(owner, rootfile);
	} else {
		m_md5sum = CalcMd5(owner);
		wxZipEntry * info = owner.GetInfo(m_filename);
		if (info && owner.OpenEntry(*info)) {
			m_parser = new FbImportReaderFB2(owner);
		}
	}
}

FbImportBook::~FbImportBook()
{
	wxDELETE(m_parser);
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

	return Md5(md5);
}

int FbImportBook::FindByMD5()
{
	return m_database.Int(m_md5sum, wxT("SELECT id FROM books WHERE md5sum=?"));
}

bool FbImportBook::AppendBook()
{
	if (!m_parser) return false;

	m_parser->Convert(m_database);

	bool ok = true;

	int id_book = - m_database.NewId(DB_NEW_BOOK);
	int today = FbDateTime::Today().Code();

	int prior;
	const AuthorArray & authors = m_parser->m_authors;
	for (size_t i = 0; i < authors.Count(); i++) {
		int author = authors[i].GetId();
		if (i && prior == author) {
			wxLogWarning(_("Dublicate author: %s %s"), authors[i].GetFullName().c_str(), m_message.c_str());
			continue;
		}
		prior = author;
		wxString sql = wxT("INSERT INTO books(id,id_archive,id_author,title,genres,file_name,file_path,file_size,file_type,lang,description,created,md5sum) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");
		FbSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id_book);
		stmt.Bind(2, m_archive);
		stmt.Bind(3, author);
		stmt.Bind(4, m_parser->m_title);
		stmt.Bind(5, m_parser->m_genres);
		stmt.Bind(6, m_filename);
		stmt.Bind(7, m_filepath);
		stmt.Bind(8, (wxLongLong)m_filesize);
		stmt.Bind(9, m_filetype);
		stmt.Bind(10, m_parser->m_lang);
		stmt.Bind(11, m_parser->m_dscr);
		stmt.Bind(12, today);
		stmt.Bind(13, m_md5sum);
		ok = stmt.ExecuteUpdate() && ok;
	}

	const SequenceArray & sequences = m_parser->m_sequences;
	for (size_t i = 0; i < sequences.Count(); i++) {
		const SequenceItem & sequence = sequences[i];
		wxString sql = wxT("INSERT INTO bookseq(id_book,id_seq,number) VALUES (?,?,?)");
		FbSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id_book);
		stmt.Bind(2, sequence.GetId());
		stmt.Bind(3, sequence.GetNumber());
		ok = stmt.ExecuteUpdate() && ok;
	}

	{
		wxString sql = wxT("INSERT INTO fts_book(content,docid) VALUES(LOW(?),?)");
		FbSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_parser->m_title);
		stmt.Bind(2, id_book);
		ok = stmt.ExecuteUpdate() && ok;
	}

	const wxString & genres = m_parser->m_genres;
	if (m_database.TableExists(wxT("genres"))) {
		size_t count = genres.Length() / 2;
		for (size_t i = 0; i < count; i++) {
			wxString sql = wxT("INSERT INTO genres(id_book, id_genre) VALUES(?,?)");
			FbSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, id_book);
			stmt.Bind(2, genres.Mid(i*2, 2));
			ok = stmt.ExecuteUpdate() && ok;
		}
	}

	return ok;
}
bool FbImportBook::AppendFile(int id_book)
{
	wxString sql = wxT("SELECT file_name FROM books WHERE id=?1 AND id_archive=?2 UNION SELECT file_name FROM files WHERE id_book=?1 AND id_archive=?2");
	FbSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id_book);
	stmt.Bind(2, m_archive);
	FbSQLite3ResultSet result = stmt.ExecuteQuery();
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
	if (!m_ok || m_md5sum.IsEmpty()) return false;

	int id_book = FindByMD5();

	if (m_parser && m_parser->IsOk()) {
		return id_book ? AppendFile(id_book) : AppendBook();
	} else {
		if (id_book == 0) wxLogMessage(_("Skip entry: %s"), m_filename.c_str());
		return id_book && AppendFile(id_book);
	}
}

//-----------------------------------------------------------------------------
//  FbRootReaderEPUB::RootHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbRootReaderEPUB::RootHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	return name == wxT("rootfiles") ? new FileHandler(m_reader, name) : NULL;
}

//-----------------------------------------------------------------------------
//  FbRootReaderEPUB::FileHandler
//-----------------------------------------------------------------------------

FbHandlerXML * FbRootReaderEPUB::FileHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	if (name == wxT("rootfile")) {
		m_reader.m_rootfile = Value(atts, wxT("full-path"));
		m_reader.Stop();
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbRootReaderEPUB
//-----------------------------------------------------------------------------

FbRootReaderEPUB::FbRootReaderEPUB(wxInputStream & in)
	: m_ok(false)
{
	{
		wxZipInputStream zip(in);
		while (FbSmartPtr<wxZipEntry> entry = zip.GetNextEntry()) {
			if (entry->GetInternalName() == wxT("META-INF/container.xml")) {
				m_ok = zip.OpenEntry(*entry) && Parse(zip);
				break;
			}
		}
	}
	in.SeekI(0);
}

FbHandlerXML * FbRootReaderEPUB::CreateHandler(const wxString &name)
{
	return name == wxT("container") ? new RootHandler(*this, name) : NULL;
}

//-----------------------------------------------------------------------------
//  FbDataReaderEPUB::RootHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbDataReaderEPUB::RootHandler)
	KEY( "dc-metadata" , Metadata );
	KEY( "metadata"    , Metadata );
FB2_END_KEYHASH

FbHandlerXML * FbDataReaderEPUB::RootHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Metadata: return new MetaHandler(m_reader, name);
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
//  FbDataReaderEPUB::MetaHandler
//-----------------------------------------------------------------------------

FB2_BEGIN_KEYHASH(FbDataReaderEPUB::MetaHandler)
	KEY( "dc-metadata"    , Metadata );
	KEY( "metadata"    , Metadata );
	KEY( "creator"     , Author );
	KEY( "title"       , Title  );
	KEY( "description" , Descr  );
	KEY( "language"    , Lang   );
FB2_END_KEYHASH

static bool IsAut(const FbStringHash &atts)
{
	wxString role = FbHandlerXML::Value(atts, wxT("role"));
	return role == wxT("aut") || role == wxT("author");
}

FbHandlerXML * FbDataReaderEPUB::MetaHandler::NewNode(const wxString &name, const FbStringHash &atts)
{
	switch (toKeyword(name)) {
		case Metadata : return new MetaHandler( m_reader, name );
		case Title    : return new TextHandler( name, m_reader.m_title );
		case Descr    : return new TextHandler( name, m_reader.m_dscr  );
		case Lang     : return new TextHandler( name, m_reader.m_lang  );
		case Author   : return IsAut(atts) ? new AuthorHandler(m_reader, name) : NULL;
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
//  FbDataReaderEPUB::AuthorHandler
//-----------------------------------------------------------------------------

FbDataReaderEPUB::AuthorHandler::AuthorHandler(FbImportReader &reader, const wxString &name)
	: FbHandlerXML(name), m_author(new AuthorItem)
{
	reader.m_authors.Add(m_author);
}

void FbDataReaderEPUB::AuthorHandler::EndNode(const wxString &name)
{
	m_text.Trim(false).Trim(true);
	size_t pos = m_text.find_last_of(wxT(' '));
	if (pos == (size_t)wxNOT_FOUND) {
		m_author->last = m_text;
	} else {
		m_author->last = m_text.Mid(pos + 1);
		m_author->first = m_text.Left(pos);
	}
}

//-----------------------------------------------------------------------------
//  FbDataReaderEPUB
//-----------------------------------------------------------------------------

FbDataReaderEPUB::FbDataReaderEPUB(wxInputStream & in, const wxString & rootfile)
{
	wxZipInputStream zip(in);
	while (FbSmartPtr<wxZipEntry> entry = zip.GetNextEntry()) {
		if (entry->GetInternalName() == rootfile) {
			m_ok = zip.OpenEntry(*entry) && Parse(zip);
			break;
		}
	}
}

FbHandlerXML * FbDataReaderEPUB::CreateHandler(const wxString &name)
{
	return name == wxT("package") ? new RootHandler(*this, name) : NULL;
}

