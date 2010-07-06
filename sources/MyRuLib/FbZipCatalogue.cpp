#include "FbZipCatalogue.h"
#include "FbCollection.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

//-----------------------------------------------------------------------------
//  FbZipTraverser
//-----------------------------------------------------------------------------

class FbZipTraverser : public wxDirTraverser
{
	public:
		FbZipTraverser(FbThread & owner): m_owner(owner) {}
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
	private:
		void AddZip(wxFileName filename);
		FbCommonDatabase m_database;
		FbThread & m_owner;
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
	{
		wxString sql = wxT("SELECT file FROM zip_files WHERE path=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, fullname);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return ;
	}

	wxULongLong size = filename.GetSize();
	if (size == wxInvalidOffset || size > 0xFFFFFFFF) {
		wxLogMessage(_("Invalid zip offset %s"), fullname.c_str());
		return;
	}

	wxLogMessage(_("Scan zip %s"), fullname.c_str());
	wxSQLite3Transaction trans(&m_database);
	int id = m_database.NewId(DB_NEW_ZIPFILE);
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
//  FbZipCatalogueThread
//-----------------------------------------------------------------------------

void * FbZipCatalogueThread::Entry()
{
	Sleep(3000);

	if (IsClosed()) return NULL;

	wxLogMessage(_("Start scan directory %s"), m_dirname.c_str());

	wxDir dir(m_dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), m_dirname.c_str());
		return NULL;
	}

	FbZipTraverser traverser(*this);
	dir.Traverse(traverser, wxT("*.zip"));

	if (!IsClosed()) m_owner.EmptyInfo();

	wxLogMessage(_("Finish scan directory %s"), m_dirname.c_str());

	return NULL;
}

