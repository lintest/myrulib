#include "FbExtractInfo.h"
#include "FbImportReader.h"
#include "FbSmartPtr.h"
#include "FbParams.h"
#include "FbString.h"
#include "FbConst.h"
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

WX_DEFINE_OBJARRAY(FbExtractArrayBase);

FbExtractItem::FbExtractItem(FbSQLite3ResultSet & result, int id, const wxString & ext, const wxString & md5):
	id_book(id),
	id_archive(result.GetInt(wxT("id_archive"))),
	book_name(result.GetString(wxT("file_name"))),
	book_path(result.GetString(wxT("file_path"))),
	file_type(ext),
	librusec(false)
{
	librusec = id_book>0 && (result.GetInt(wxT("file")) == 0);
	if (librusec) {
		if (FbParamItem::IsGenesis()) {
			book_name << id / 1000 * 1000 << wxT('/') << Lower(md5);
		} else if (book_name.IsEmpty()) {
			book_name << id << wxT('.') << ext;
		}
	}
}

FbExtractItem::FbExtractItem(const FbExtractItem & item):
	id_book(item.id_book),
	id_archive(item.id_archive),
	book_name(item.book_name),
	book_path(item.book_path),
	librusec(item.librusec)
{
}

bool FbExtractItem::NotFb2() const
{
	return Ext(book_name) != wxT("fb2");
}

wxString FbExtractItem::InfoName() const
{
	int pos = book_name.rfind(wxT('.'));
	wxString result;
	if (pos == wxNOT_FOUND) {
		result = book_name;
	} else {
		result = book_name.substr(0, pos);
	}
	return result << wxT(".fbd");
}

wxString FbExtractItem::FileName(bool bInfoOnly) const
{
	if (bInfoOnly && NotFb2())
		return InfoName();
	else return book_name;
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

wxString FbExtractItem::ErrorName() const
{
	if (librusec) {
		return wxString::Format(wxT("$(%s)/%s"), FbParams(FB_CONFIG_TYPE).Str().c_str(), book_name.c_str());
	} else if ( id_archive ) {
		if ( wxFileName(zip_name).GetName() == book_name )
			return zip_name.c_str();
		else {
			wxString result = zip_name;
			result << wxT(": ");
			FbString filename = book_name;
			if (filename.Len() > 0x24) {
				wxString ext;
				int pos = filename.Find(wxT('.'), true);
				if (pos != wxNOT_FOUND) ext = filename.Mid(pos);
				filename.Truncate(pos);
				filename = filename.AfterLast(wxT('/'));
				filename.Shorten() << ext;
			}
			return result << filename;
		}
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
		FbSmartPtr<wxZipEntry> entry;
		while (entry = zip.GetNextEntry()) {
			if (entry->GetSize()) {
				if (Ext(entry->GetInternalName()) != wxT("fbd")) count++;
				if (count > 1) return;
			}
		}
	}

	FbLogWarning(_("Delete"), ErrorName());
	wxRemoveFile(filename.GetFullPath());
}

wxString FbExtractItem::GetURL() const
{
	return id_archive ? zip_name : book_name;
}

FbExtractArray::FbExtractArray(FbSQLite3Database & database, const int id)
	:FbExtractArrayBase(), m_id(id)
{
	{
		wxString sql = wxT("\
			SELECT DISTINCT 0 AS file, id, id_archive, file_name, file_path, file_type, md5sum FROM books WHERE id=? UNION ALL \
			SELECT DISTINCT 1 AS file, id_book, id_archive, file_name, file_path, NULL, NULL FROM files WHERE id_book=? \
			ORDER BY file \
		");
		FbSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		stmt.Bind(2, id);
		FbSQLite3ResultSet result = stmt.ExecuteQuery();
		wxString filetype;
		wxString md5sum;
		while ( result.NextRow() ) {
			if (filetype.IsEmpty()) filetype = result.GetString(wxT("file_type"));
			if (md5sum.IsEmpty()) md5sum = result.GetString(wxT("md5sum"));
			Add(FbExtractItem(result, id, filetype, md5sum));
		}
	}

	{
		wxString sql = wxT("SELECT file_name, file_path FROM archives WHERE id=?");
		for (size_t i = 0; i<Count(); i++) {
			FbExtractItem & item = Item(i);
			if (!item.id_archive) continue;
			FbSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, item.id_archive);
			FbSQLite3ResultSet result = stmt.ExecuteQuery();
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

