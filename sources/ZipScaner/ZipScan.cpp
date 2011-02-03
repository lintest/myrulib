#include <stdio.h>
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/filename.h>
#include <wx/wxsqlite3.h>
#include "../MyRuLib/polarssl/md5.h"
#include "../MyRuLib/wx/base64.h"

class FbZipTraverser : public wxDirTraverser
{
	public:
		FbZipTraverser(wxSQLite3Database & database);
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
	private:
		void AddZip(wxFileName filename);
		wxString CalcMd5(wxInputStream& stream);
	private:
		wxSQLite3Database & m_database;
};

FbZipTraverser::FbZipTraverser(wxSQLite3Database & database)
	: m_database(database)
{
	wxString sql[] = {
		wxT("CREATE TABLE IF NOT EXISTS zip(zip INTEGER PRIMARY KEY, file VARCHAR(255))"),
		wxT("CREATE TABLE IF NOT EXISTS entry(zip INTEGER, name VARCHAR(255), md5 CHAR(32), PRIMARY KEY(zip, name))"),
		wxT("CREATE INDEX IF NOT EXISTS zip_file ON zip(file)"),
	};
	size_t count = sizeof(sql) / sizeof(wxString);

	for (size_t i = 0; i < count; i++) {
		m_database.ExecuteUpdate(sql[i]);
	}
}

wxDirTraverseResult FbZipTraverser::OnFile(const wxString& filename)
{
	AddZip(filename);
	return wxDIR_CONTINUE;
}

wxDirTraverseResult FbZipTraverser::OnDir(const wxString& WXUNUSED(dirname))
{
	return wxDIR_IGNORE;
}

wxString FbZipTraverser::CalcMd5(wxInputStream& stream)
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

	wxString md5sum;
	unsigned char output[16];
	md5_finish( &md5, output );
	for (size_t i=0; i<16; i++)
		md5sum += wxString::Format(wxT("%02x"), output[i]);
	return md5sum;
}

void FbZipTraverser::AddZip(wxFileName filename)
{
	wxSQLite3Transaction trans(&m_database);

	wxString fullname = filename.GetFullName();

	wxULongLong size = filename.GetSize();
	if (size == wxInvalidOffset || size > 0xFFFFFFFF) {
		wxLogError(_("Invalid zip offset %s"), fullname.c_str());
		return;
	}

	wxString sql_find = wxT("SELECT zip FROM zip WHERE file=?");

	{
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql_find);
		stmt.Bind(1, fullname);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return;
	}

	{
		wxString sql = wxT("INSERT INTO zip(file) values(?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, fullname);
		stmt.ExecuteUpdate();
	}

	int id = 0;
	{
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql_find);
		stmt.Bind(1, fullname);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) id = result.GetInt(0);
	}
	if (id == 0) return;

	wxLogMessage(fullname.c_str());
	wxFFileInputStream in(filename.GetFullPath());
	wxZipInputStream zip(in);

	int count = 0;
	{
		wxString sql = wxT("INSERT INTO entry(zip, name, md5) values(?,?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		
		while (wxZipEntry * entry = zip.GetNextEntry()) {
			if (entry->GetSize()) {
				wxString msg = wxT("\t"); msg << entry->GetName();
				wxLogMessage(msg);
				zip.OpenEntry(*entry);
				wxString md5 = CalcMd5(zip);

				stmt.Bind(1, id);
				stmt.Bind(2, entry->GetName());
				stmt.Bind(3, md5);
				stmt.ExecuteUpdate();

				count++;
			}
			delete entry;
		}
	}

	if (count) trans.Commit();
}

void GenerateScript(wxSQLite3Database & database, const wxString & filename)
{
	wxFileOutputStream stream(filename);
	wxTextOutputStream out(stream);

	out << wxT("START TRANSACTION;\n");

	out << wxT("\
DROP TABLE IF EXISTS myrulib_zip; \n\
CREATE TABLE myrulib_zip ( \n\
  zid int(10) PRIMARY KEY, \n\
  file varchar(255) COLLATE utf8_unicode_ci NOT NULL \n\
) DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci; \n\
");

	out << wxT("\
DROP TABLE IF EXISTS myrulib_entry; \n\
CREATE TABLE myrulib_entry ( \n\
  zid int(10), \n\
  name varchar(255) COLLATE utf8_unicode_ci NOT NULL, \n\
  md5 char(32) COLLATE utf8_unicode_ci NOT NULL, \n\
  PRIMARY KEY (zid, name), \n\
  KEY md5 (md5) \n\
) DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci; \n\
");

	{
		out << wxT("\nLOCK TABLES myrulib_zip WRITE;\n");
		wxString sql = wxT("SELECT zip, file FROM zip");
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		while (result.NextRow()) {
			out << wxString::Format( wxT("INSERT INTO myrulib_zip VALUES (%d,\"%s\");"), result.GetInt(0), result.GetString(1).c_str() );
			out << wxT('\n');
		}
		out << wxT("UNLOCK TABLES;\n");
	}

	{
		out << wxT("\nLOCK TABLES myrulib_entry WRITE;\n");
		wxString sql = wxT("SELECT zip, name, md5 FROM entry");
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		while (result.NextRow()) {
			wxString name = result.GetString(1);
			if (name.IsEmpty()) continue;
			name.Replace(wxT("'"), wxT("\\'"));
			name.Replace(wxT("\""), wxT("\\\""));
			wxString md5s = result.GetString(2);
			out << wxString::Format( wxT("INSERT INTO myrulib_entry VALUES (%d,\"%s\",\"%s\");\n"), result.GetInt(0), name.c_str(), md5s.c_str() );
		}
		out << wxT("UNLOCK TABLES;\n");
	}

	out << wxT("COMMIT;\n");

}

int main(int argc, char **argv)
{
	wxFileName filename = argc ? wxString(argv[0], wxConvUTF8) :wxT("zipscan");
	filename.SetExt(wxT("db"));

	wxString dirname = filename.GetPath();
	wxDir dir(dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), dirname.c_str());
		return 1;
	}

	wxSQLite3Database database;
	database.Open(filename.GetFullPath());

	FbZipTraverser traverser(database);
	dir.Traverse(traverser, wxT("*.zip"), wxDIR_FILES);

	filename.SetExt(wxT("sql"));
	GenerateScript(database, filename.GetFullPath());

	return 0;
}
