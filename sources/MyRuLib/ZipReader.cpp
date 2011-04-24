#include "ZipReader.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbExtractInfo.h"
#include "FbDatabase.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbDownloader.h"
#include "FbCollection.h"
#include <wx/wxsqlite3.h>

ZipReader::ZipReader(int id, bool bShowError, bool bInfoOnly)
	:conv(wxT("cp866")), m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
	FbCommonDatabase database;

	OpenDownload(database, bInfoOnly);
	if (IsOk()) return;

	FbExtractArray items(database, id);

	wxString error_name;
	wxString sLibraryDir = wxGetApp().GetLibPath();

	for (size_t i = 0; i<items.Count(); i++) {
		FbExtractItem & item = items[i];
		wxString file_name = item.FileName(bInfoOnly);
		if (i==0) error_name = item.ErrorName();
		if (item.id_archive) {
			wxFileName zip_file;
			m_zipOk = item.FindZip(sLibraryDir, zip_file);
			if (m_zipOk) OpenZip(zip_file.GetFullPath(), file_name);
		} else if (item.librusec) {
			if (FbParams::IsGenesis()) {
				wxFileName book_file = sLibraryDir + wxT('/') + file_name;
				m_zipOk = book_file.IsOk() && book_file.FileExists();
				if (m_zipOk) OpenFile(book_file.GetFullPath());
			}
		} else {
			wxFileName book_file;
			m_zipOk = item.FindBook(sLibraryDir, book_file);
			if (m_zipOk) OpenFile(book_file.GetFullPath());
		}
		if (IsOk()) return;
	}
	if (bShowError) wxLogError(_("Book not found %s"), error_name.c_str());
}

ZipReader::~ZipReader()
{
	wxDELETE(m_zip);
	wxDELETE(m_file);
}

void ZipReader::OpenDownload(FbDatabase &database, bool bInfoOnly)
{
	wxString filetype;
	{
		wxString sql = wxT("SELECT md5sum, file_type FROM books WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if ( result.NextRow() ) {
			m_md5sum = result.GetString(0);
			filetype = result.GetString(1).Lower();
			if (filetype == wxT("fb2")) bInfoOnly = false;
		} else return;
	}

	wxFileName zip_file = FbDownloader::GetFilename(m_md5sum, false);
	m_zipOk = zip_file.FileExists();
	if (m_zipOk && !bInfoOnly) {
		m_file = new wxFFileInputStream(zip_file.GetFullPath());
		if ( filetype != wxT("zip") ) {
			unsigned char buf[2];
			size_t count = m_file->Read(buf, 2).LastRead();
			if ( count>1 && buf[0]=='P' && buf[1]=='K') {
				OpenEntry(bInfoOnly);
				if ( IsOk() ) return;
			}
			m_file->SeekI(0);
		}
		wxDELETE(m_zip);
		m_result = m_file;
		m_fileOk = true;
		return;
	}

	zip_file.SetExt(wxT("zip"));
	m_zipOk = zip_file.FileExists();
	if (m_zipOk) {
		m_file = new wxFFileInputStream(zip_file.GetFullPath());
		m_zip = new wxZipInputStream(*m_file, conv);
		m_result = m_zip;
		OpenEntry(bInfoOnly);
	}
}

void ZipReader::OpenEntry(bool bInfoOnly)
{
	m_zip = new wxZipInputStream(*m_file, conv);
	m_result = m_zip;
	while (wxZipEntry * entry = m_zip->GetNextEntry()) {
		bool ok = (entry->GetInternalName().Right(4).Lower() == wxT(".fbd")) == bInfoOnly;
		if (ok) m_fileOk = m_zip->OpenEntry(*entry);
		delete entry;
		if (ok) break;
	}
}

void ZipReader::OpenZip(const wxString &zipname, const wxString &filename)
{
	m_file = new wxFFileInputStream(zipname);
	m_zip = new wxZipInputStream(*m_file, conv);
	m_result = m_zip;

	m_zipOk  = m_file->IsOk();
	m_fileOk = m_zipOk && FindEntry(filename);

	wxString zipText = (zipname.IsNull() ? zipname : wxT(" ") + zipname );
	wxString fileText = (filename.IsNull() ? filename : wxT(" ") + filename );
	m_info = wxString::Format(_("Not found an archive (%s), containing file (%s)."), zipText.c_str(), fileText.c_str());
}

void ZipReader::OpenFile(const wxString &filename)
{
	m_file = new wxFFileInputStream(filename);
	m_zip = NULL;
	m_result = m_file;

	m_zipOk  = true;
	m_fileOk = m_file->IsOk();

	wxString zipText = wxEmptyString;
	wxString fileText = (filename.IsNull() ? filename : wxT(" ") + filename );
	m_info = wxString::Format(_("Not found file %s"), fileText.c_str());
}

bool ZipReader::FindEntry(const wxString &file_name)
{
	bool find_ok = false;
	bool open_ok = false;
	while (wxZipEntry * entry = m_zip->GetNextEntry()) {
		find_ok = (entry->GetInternalName() == file_name);
		if (find_ok) open_ok = m_zip->OpenEntry(*entry);
		delete entry;
		if (find_ok) break;
	}
	return find_ok && open_ok;
}

void ZipReader::ShowError()
{
	wxMessageBox(GetErrorText());
}

