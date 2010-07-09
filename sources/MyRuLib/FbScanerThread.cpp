#include "FbScanerThread.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

//-----------------------------------------------------------------------------
//  FbZipTraverser
//-----------------------------------------------------------------------------

class FbCountTraverser : public wxDirTraverser
{
	public:
		FbCountTraverser(): m_count(0) { }
		virtual wxDirTraverseResult OnFile(const wxString& filename) {
			m_count++;
			return wxDIR_CONTINUE;
		}
		virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) {
			return wxDIR_IGNORE;
		}
		unsigned int GetCount() { return m_count; }
	private:
		unsigned int m_count;
};

//-----------------------------------------------------------------------------
//  FbZipTraverser
//-----------------------------------------------------------------------------

class FbZipTraverser : public wxDirTraverser
{
	public:
		FbZipTraverser(FbScanerThread & owner, bool only_new)
			: m_owner(owner), m_database(owner.GetDatabase()), m_only_new(only_new) {}
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
	private:
		void AddZip(wxFileName filename);
		int FindFile(const wxString &name);
		int NewFile(const wxString &name);
		bool FindBook(int id, const wxString &name);
		void NewBook(int id, const wxString &name);
	private:
		FbScanerThread & m_owner;
		FbDatabase & m_database;
		bool m_only_new;
};

wxDirTraverseResult FbZipTraverser::OnFile(const wxString& filename)
{
	AddZip(filename);
	return m_owner.IsClosed() ? wxDIR_STOP : wxDIR_CONTINUE;
}

wxDirTraverseResult FbZipTraverser::OnDir(const wxString& WXUNUSED(dirname))
{
	return m_owner.IsClosed() ? wxDIR_STOP : wxDIR_IGNORE;
}

int FbZipTraverser::FindFile(const wxString &name)
{
	wxString sql = wxT("SELECT file FROM zip_files WHERE path=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, name);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0) : 0;
}

bool FbZipTraverser::FindBook(int id, const wxString &name)
{
	wxString sql = wxT("SELECT file FROM zip_books WHERE file=? AND book=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id);
	stmt.Bind(1, name);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow();
}

int FbZipTraverser::NewFile(const wxString &name)
{
	int id = m_database.NewId(DB_NEW_ZIPFILE);
	wxString sql = wxT("INSERT INTO zip_files(file,path) values(?,?)");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id);
	stmt.Bind(2, name);
	stmt.ExecuteUpdate();
	return id;
}

void FbZipTraverser::NewBook(int id, const wxString &name)
{
	wxString sql = wxT("INSERT INTO zip_books(file,book) values(?,?)");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id);
	stmt.Bind(2, name);
	stmt.ExecuteUpdate();
}

void FbZipTraverser::AddZip(wxFileName filename)
{
	wxString fullname = filename.GetFullName();
	m_owner.Progress1(fullname);

	wxULongLong size = filename.GetSize();
	if (size == wxInvalidOffset || size > 0xFFFFFFFF) {
		wxLogError(_("Invalid zip offset %s"), fullname.c_str());
		return;
	}

	wxLogMessage(_("Scan zip %s"), fullname.c_str());
	wxFFileInputStream in(filename.GetFullPath());
	wxZipInputStream zip(in);

	int id = FindFile(fullname);
	if (m_only_new && id) return;

	wxSQLite3Transaction trans(&m_database);
	if (!id) id = NewFile(fullname);

	int max = zip.GetTotalEntries();
	int pos = 0;
	int num = 0;

	int count = 0;
	{
		wxString sql = wxT("INSERT INTO zip_books(file,book) values(?,?)");
		while (wxZipEntry * entry = zip.GetNextEntry()) {
			int new_pos = (++num) * 100 / max;
			if (new_pos != pos) m_owner.Progress2(pos = new_pos);

			if (entry->GetSize()) {
				wxString name = entry->GetName(wxPATH_UNIX);
				bool exists = FindBook(id, name);
				if (!exists) NewBook(id, name);
				count++;
			}
			delete entry;
		}
	}

	if (count) {
		trans.Commit();
	} else wxLogError(_("Zip read error %s"), fullname.c_str());
}

//-----------------------------------------------------------------------------
//  FbScanerThread
//-----------------------------------------------------------------------------

void FbScanerThread::SavePath()
{
	wxFileName relative = m_dirname;
	relative.MakeRelativeTo(m_filename.GetPath());
	wxString path = relative.GetFullPath();
	relative.MakeAbsolute(m_filename.GetPath());
	if (relative != m_dirname) path = m_dirname.GetFullPath();
	m_database.SetText(DB_LIBRARY_DIR, path);
}

void * FbScanerThread::Entry()
{
	int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX;
	m_database.Open(m_filename.GetFullPath(), wxEmptyString, flags);

	if (!m_database.IsOpen()) return NULL;

	if (IsClosed()) return NULL;
	SavePath();

	wxString dirname = m_dirname.GetFullPath();

	wxLogMessage(_("Start scan directory %s"), dirname.c_str());

	wxDir dir(dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), dirname.c_str());
		return NULL;
	}

	FbCountTraverser counter;
	dir.Traverse(counter);
	m_max = counter.GetCount();
	m_pos = 0;

	FbZipTraverser traverser(*this, m_only_new);
	dir.Traverse(traverser, wxT("*.zip"));

	wxLogMessage(_("Finish scan directory %s"), dirname.c_str());

	return NULL;
}

void FbScanerThread::Progress1(const wxString & text)
{
	m_pos++;
	int pos = m_pos * 100 / m_max;
	FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_1, pos, text).Post(m_owner);
}

void FbScanerThread::Progress2(int position)
{
	FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_2, position).Post(m_owner);
}

void FbScanerThread::OnExit()
{
	FbCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK).Post(m_owner);
}
