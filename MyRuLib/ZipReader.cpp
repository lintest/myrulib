#include "ZipReader.h"
#include "FbParams.h"
#include "MyRuLibApp.h"

ZipReader::ZipReader(int id)
    :m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
    wxFileName zip_name;
    {
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

        wxString path = FbParams::GetText(FB_LIBRARY_DIR);

        Books books(wxGetApp().GetDatabase());
        BooksRow * bookRow = books.Id(id);

        if (!bookRow) return;

        m_file_name = bookRow->file_name;

        if (bookRow->id_archive) {
            ArchivesRow * archiveRow = bookRow->GetArchive();
            if (!archiveRow) return;
            m_zip_name = archiveRow->file_name;
            zip_name = archiveRow->file_name;
            zip_name.SetPath(archiveRow->file_path);
            m_zipOk = FindZip(zip_name, path);
            if (m_zipOk) {
                OpenZip(zip_name.GetFullPath());
            }
        } else {
            wxFileName filename = bookRow->file_name;
            if (filename.FileExists()) {
                OpenFile(filename.GetFullPath());
            } else {
                wxString whereClause = wxString::Format(wxT("min_id_book<=%d AND %d<=max_id_book"), id, id);
                Archives archives(wxGetApp().GetDatabase());
                ArchivesRowSet * archiveRowSet = archives.WhereSet(whereClause);
                for (size_t i=0; i<archiveRowSet->Count(); i++) {
                    ArchivesRow * archiveRow = archiveRowSet->Item(i);
                    m_zip_name = archiveRow->file_name;
                    zip_name = archiveRow->file_name;
                    zip_name.SetPath(archiveRow->file_path);
                    m_zipOk = FindZip(zip_name, path);
                    if (m_zipOk) {
                        OpenZip(zip_name.GetFullPath());
                        break;
                    }
                }
            }
        }
    }
}

ZipReader::~ZipReader()
{
	wxDELETE(m_zip);
	wxDELETE(m_file);
}

void ZipReader::OpenZip(const wxString &zipname)
{
    m_file = new wxFFileInputStream(zipname);
    m_zip = new wxZipInputStream(*m_file);
    m_result = m_zip;

    m_zipOk  = m_file->IsOk();
    m_fileOk = FindEntry(m_file_name);
}

void ZipReader::OpenFile(const wxString &filename)
{
    m_file = new wxFFileInputStream(filename);
    m_zip = NULL;
    m_result = m_file;

    m_zipOk  = true;
    m_fileOk = m_file->IsOk();
}

bool ZipReader::FindZip(wxFileName &zip_name, wxString &path)
{
    if (zip_name.FileExists()) return true;

    zip_name.SetPath(path);
    if (zip_name.FileExists()) return true;

    zip_name.SetPath(wxGetApp().GetAppPath());
    if (zip_name.FileExists()) return true;

    return false;
}

bool ZipReader::FindEntry(const wxString &file_name)
{
	bool find_ok = false;
	bool open_ok = false;
	while (wxZipEntry * entry = m_zip->GetNextEntry()) {
	    find_ok = (entry->GetName() == file_name);
		if (find_ok) open_ok = m_zip->OpenEntry(*entry);
		delete entry;
		if (find_ok) break;
	}
	return find_ok && open_ok;
}

extern wxString strBookNotFound;

void ZipReader::ShowError()
{
    wxMessageBox(GetErrorText());
}

wxString ZipReader::GetErrorText()
{
    wxString zipText = (m_zip_name.IsNull() ? m_zip_name : wxT(" ") + m_zip_name );
    wxString fileText = (m_file_name.IsNull() ? m_file_name : wxT(" ") + m_file_name );
    return wxString::Format(strBookNotFound, zipText.c_str(), fileText.c_str());
}
