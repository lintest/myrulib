#include "FbScanerThread.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

//-----------------------------------------------------------------------------
//  FbZipTraverser
//-----------------------------------------------------------------------------

class FbZipTraverser : public wxDirTraverser
{
	public:
		FbZipTraverser(FbScanerThread & owner): m_owner(owner), m_database(owner.GetDatabase()) {}
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
	private:
		void AddZip(wxFileName filename);
	private:
		FbScanerThread & m_owner;
		FbDatabase & m_database;
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

void FbZipTraverser::AddZip(wxFileName filename)
{
	wxString fullname = filename.GetFullName();
	m_owner.Progress(fullname);

	int id = 0;
	{
		wxString sql = wxT("SELECT file FROM zip_files WHERE path=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, fullname);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) id = result.GetInt(0);
	}

	wxULongLong size = filename.GetSize();
	if (size == wxInvalidOffset || size > 0xFFFFFFFF) {
		wxLogMessage(_("Invalid zip offset %s"), fullname.c_str());
		return;
	}

	wxLogMessage(_("Scan zip %s"), fullname.c_str());
	wxSQLite3Transaction trans(&m_database);
	if (!id) int id = m_database.NewId(DB_NEW_ZIPFILE);
	wxFFileInputStream in(filename.GetFullPath());
	wxZipInputStream zip(in);

	int count = 0;
	{
		wxString sql = wxT("INSERT INTO zip_books(file,book) values(?,?)");
		while (wxZipEntry * entry = zip.GetNextEntry()) {
			if (entry->GetSize()) {
				wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
				stmt.Bind(1, id);
				stmt.Bind(2, entry->GetName(wxPATH_UNIX));
				stmt.ExecuteUpdate();
				count++;
			}
			delete entry;
		}
	}

	if (count) {
		wxString sql = wxT("INSERT INTO zip_files(file,path) values(?,?)");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, id);
		stmt.Bind(2, fullname);
		stmt.ExecuteUpdate();
		trans.Commit();
	} else {
		wxLogError(_("Zip read error %s"), fullname.c_str());
	}
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

	if (IsClosed()) return NULL;
	SavePath();

	wxString dirname = m_dirname.GetFullPath();

	wxLogMessage(_("Start scan directory %s"), dirname.c_str());

	wxDir dir(dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), dirname.c_str());
		return NULL;
	}

	FbZipTraverser traverser(*this);
	dir.Traverse(traverser, wxT("*.zip"));

	wxLogMessage(_("Finish scan directory %s"), dirname.c_str());

	return NULL;
}

void FbScanerThread::Progress(const wxString & text)
{
	FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_UPDATE, text).Post(m_owner);
}
