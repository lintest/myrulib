#include <stdio.h>
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/filename.h>
#include <wx/wxsqlite3.h>
#include "../MyRuLib/polarssl/md5.h"
#include "../MyRuLib/wx/base64.h"

class FbZipTraverser : public wxDirTraverser
{
	public:
		FbZipTraverser(const wxString &filename);
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
	private:
		void AddZip(wxFileName filename);
		wxString CalcMd5(wxInputStream& stream);
	private:
		wxSQLite3Database m_database;
};

FbZipTraverser::FbZipTraverser(const wxString &filename)
{
	m_database.Open(filename);

	wxString sql[] = {
		wxT("CREATE TABLE IF NOT EXISTS zip(zip INTEGER PRIMARY KEY, file VARCHAR(255))"),
		wxT("CREATE TABLE IF NOT EXISTS entry(zip INTEGER, name VARCHAR(255), md5 CHAR(32))"),
		wxT("CREATE INDEX IF NOT EXISTS zip_file ON zip(file)"),
		wxT("CREATE INDEX IF NOT EXISTS entry_zip ON entry(zip)"),
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
		while (wxZipEntry * entry = zip.GetNextEntry()) {
			if (entry->GetSize()) {
				wxString msg = wxT("\t"); msg << entry->GetName();
				wxLogMessage(msg);
				zip.OpenEntry(*entry);
				wxString md5 = CalcMd5(zip);

				wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
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

	FbZipTraverser traverser(filename.GetFullPath());
	dir.Traverse(traverser, wxT("*.zip"), wxDIR_FILES);

	return 0;
}
