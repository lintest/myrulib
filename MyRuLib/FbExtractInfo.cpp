#include "FbExtractInfo.h"
#include "FbParams.h"
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

WX_DEFINE_OBJARRAY(FbExtractArrayBase);

FbExtractItem::FbExtractItem(wxSQLite3ResultSet & result):
	id_book(result.GetInt(wxT("id"))),
	id_archive(result.GetInt(wxT("id_archive"))),
	book_name(result.GetString(wxT("file_name"))),
	book_path(result.GetString(wxT("file_path"))),
	librusec(false)
{
	librusec = (id_book>0 && result.GetInt(wxT("file")) == 0);
}

wxFileName FbExtractItem::GetBook(const wxString &path) const
{
	wxFileName result = book_name;
	result.Normalize(wxPATH_NORM_ALL, path);
	return result;
}

wxFileName FbExtractItem::GetZip(const wxString &path) const
{
	wxFileName result = zip_name;
	result.Normalize(wxPATH_NORM_ALL, path);
	return result;
}

bool FbExtractItem::FindZip(const wxString &basepath, wxFileName &filename) const
{
	filename = GetZip(basepath);
	if (filename.FileExists()) return true;
	if (zip_path.IsEmpty()) return false;
	filename = zip_path;
	return filename.FileExists();
}

bool FbExtractItem::FindBook(const wxString &basepath, wxFileName &filename) const
{
	filename = GetBook(basepath);
	if (filename.FileExists()) return true;
	if (book_path.IsEmpty()) return false;
	filename = book_path;
	return filename.FileExists();
}

wxString FbExtractItem::NameInfo() const
{
	if (librusec) {
		return wxString::Format(wxT("$(%s)/%s"), FbParams::GetText(FB_CONFIG_TYPE).c_str(), book_name.c_str());
	} else if ( id_archive ) {
		if ( wxFileName(zip_name).GetName() == book_name )
			return zip_name.c_str();
		else
			return wxString::Format(wxT("%s: %s"), zip_name.c_str(), book_name.c_str());
	} else {
		return book_name.c_str();
	}
}

void FbExtractItem::DeleteFile(const wxString &basepath) const
{
	if (librusec) return;

	wxFileName filename = id_archive ? GetZip(basepath) : GetBook(basepath);

	if (!filename.FileExists()) return;

	if (id_archive) {
		wxCSConv conv(wxT("cp866"));
		wxFFileInputStream file(filename.GetFullPath());
		wxZipInputStream zip(file, conv);
		size_t count = 0;
		while (wxZipEntry * entry = zip.GetNextEntry()) {
			if (entry->GetSize()) {
				if (entry->GetInternalName().Right(4).Lower() != wxT(".fbd")) count++;
				delete entry;
				if (count>1) return;
			}
		}
	}

	wxLogWarning(_("Delete: ") + NameInfo());
	wxRemoveFile(filename.GetFullPath());
}

FbExtractArray::FbExtractArray(FbDatabase & database, const int id)
	:FbExtractArrayBase(), m_id(id)
{
	{
		wxString sql = wxT("\
			SELECT DISTINCT 0 AS file, id, id_archive, file_name, file_path FROM books WHERE id=? UNION ALL \
			SELECT DISTINCT 1 AS file, id_book, id_archive, file_name, file_path FROM files WHERE id_book=? \
			ORDER BY file \
		");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		stmt.Bind(2, id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		while ( result.NextRow() ) Add(result);
	}

	{
		wxString sql = wxT("SELECT file_name, file_path FROM archives WHERE id=?");
		for (size_t i = 0; i<Count(); i++) {
			FbExtractItem & item = Item(i);
			if (!item.id_archive) continue;
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, item.id_archive);
			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			if (result.NextRow()) {
				item.zip_name = result.GetString(wxT("file_name"));
				item.zip_path = result.GetString(wxT("file_path"));
			}
		}
	}
}

void FbExtractArray::DeleteFiles(const wxString &basepath) const
{
	for (size_t i = 0; i<Count(); i++) {
		FbExtractItem & item = Item(i);
		if (item.librusec) continue;
		item.DeleteFile(basepath);
	}
}

