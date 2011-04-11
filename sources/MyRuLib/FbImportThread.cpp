#include "FbImportThread.h"
#include "FbImportReader.h"
#include <wx/dir.h>
#include <wx/list.h>
#include "FbConst.h"
#include "FbDateTime.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "MyRuLibApp.h"
#include "polarssl/md5.h"
#include "wx/base64.h"

//-----------------------------------------------------------------------------
//  FbImportZip
//-----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbZipEntryList);

FbImpotrZip::FbImpotrZip(FbImportThread *owner, wxInputStream &in, const wxString &zipname):
	m_database(owner->m_database),
	m_conv(wxT("cp866")),
	m_zip(in, m_conv),
	m_filename(owner->GetRelative(zipname)),
	m_filepath(owner->GetAbsolute(zipname)),
	m_filesize(in.GetLength()),
	m_ok(m_zip.IsOk())
{
	if (!m_ok) {
		wxLogError(_("Zip read error %s"), zipname.c_str());
		return;
	}
	wxLogMessage(_("Import zip %s"), m_filename.c_str());

    while (wxZipEntry * entry = m_zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetInternalName();
			if (filename.Right(4).Lower() == wxT(".fbd")) {
				filename = filename.Left(filename.Length() - 4);
				wxZipEntry*& current = m_map[filename];
				delete current;
				current = entry;
			} else {
				m_list.Add(entry);
			}
		}
    }
}

wxZipEntry * FbImpotrZip::GetInfo(const wxString & filename)
{
	size_t pos = filename.Length();
	while (pos) {
		if ( filename[pos] == wxT('.') ) {
			wxString infoname = filename.Left(pos);
			FbZipEntryMap::iterator it = m_map.find(infoname);
			if (it != m_map.end()) return it->second;
			break;
		}
		pos--;
	}
	return NULL;
}

int FbImpotrZip::Save()
{
	FbAutoCommit transaction(m_database);
	{
		wxString sql = wxT("SELECT id FROM archives WHERE file_name=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_filename);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		m_id = result.NextRow() ? result.GetInt(0) : 0;
	}

	wxString sql = m_id ?
		wxT("UPDATE archives SET file_name=?,file_path=?,file_size=?,file_count=? WHERE id=?") :
		wxT("INSERT INTO archives(file_name,file_path,file_size,file_count,id) VALUES (?,?,?,?,?)") ;

	if (!m_id) m_id = - m_database.NewId(DB_NEW_ARCHIVE);

	{
		wxLongLong count = m_zip.GetTotalEntries();
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_filename);
		stmt.Bind(2, m_filepath);
		stmt.Bind(3, (wxLongLong)m_filesize);
		stmt.Bind(4, count);
		stmt.Bind(5, m_id);
		stmt.ExecuteUpdate();
	}

	return m_id;
}

void FbImpotrZip::Make(FbImportThread *owner)
{
	size_t skipped = 0;
	size_t existed = m_list.Count();
	if (owner) owner->DoStart(existed, m_filename);

	size_t processed = 0;
	for (size_t i=0; i<existed; i++) {
		wxZipEntry * entry = m_list[i];
		if (owner) owner->DoStep(entry->GetInternalName());
		FbImportBook book(this, entry);
		if (book.IsOk()) {
			book.Save();
			processed++;
		} else skipped++;
	}

	if ( existed && processed == 0 ) {
		wxLogWarning(_("FB2 and FBD not found %s"), m_filename.c_str());
	}

	if ( existed == 0 ) {
		wxLogError(_("Zip read error %s"), m_filename.c_str());
	}

	if (owner) owner->DoFinish();
}

//-----------------------------------------------------------------------------
//  FbImportThread
//-----------------------------------------------------------------------------

FbImportThread::FbImportThread():
	m_counter(m_database),
	m_basepath(wxGetApp().GetLibPath()),
	m_fullpath(FbParams::GetInt(FB_SAVE_FULLPATH))
{
}

void FbImportThread::OnExit()
{
	m_counter.Execute();
}

wxString FbImportThread::GetRelative(const wxString &filename)
{
	wxFileName result = filename;
	result.Normalize(wxPATH_NORM_ALL);
	result.MakeRelativeTo(m_basepath);
	return result.GetFullPath(wxPATH_UNIX);
}

wxString FbImportThread::GetAbsolute(const wxString &filename)
{
	return m_fullpath ? filename : (wxString)wxEmptyString;
}

//-----------------------------------------------------------------------------
//  FbZipImportThread
//-----------------------------------------------------------------------------

void *FbZipImportThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	size_t count = m_filelist.Count();
	wxLogMessage(_("Start import %d file(s)"), count);
	for (size_t i=0; i<count; i++) {
		ImportFile(m_filelist[i]);
	}
	wxLogMessage(_("Finish import %d file(s)"), count);
	return NULL;
}

void FbZipImportThread::ImportFile(const wxString & zipname)
{
	wxLogMessage(_("Import file %s"), zipname.c_str());

	wxFFileInputStream in(zipname);
	if ( !in.IsOk() ) {
		wxLogError(_("File read error %s"), zipname.c_str());
		return;
	}

	DoStart(0, zipname);

	if (zipname.Right(4).Lower() == wxT(".fb2")) {
		FbImportBook book(this, in, zipname);
		if (book.IsOk()) book.Save();
		DoFinish();
	} else {
		FbImpotrZip zip(this, in, zipname);
		if (zip.IsOk()) {
			zip.Save();
			zip.Make(this);
		}
	}
}

//-----------------------------------------------------------------------------
//  FbDirImportThread
//-----------------------------------------------------------------------------

class CountTraverser : public wxDirTraverser
{
public:
	CountTraverser() : m_count(0) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		wxString ext = filename.Right(4).Lower();
		if (ext==wxT(".fb2") || ext==wxT(".zip")) m_count++;
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) {
		return wxDIR_CONTINUE;
	}
	unsigned int GetCount() { return m_count; }
private:
	unsigned int m_count;
};

class FolderTraverser : public wxDirTraverser
{
public:
	FolderTraverser(FbDirImportThread* thread) : m_thread(thread), m_progress(0) { };

	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		wxString ext = filename.Right(4).Lower();
		if (ext == wxT(".fb2")) {
			Progress(filename);
			m_thread->ParseXml(filename);
		} else if (ext == wxT(".zip")) {
			Progress(filename);
			m_thread->ParseZip(filename);
		} else {
			wxLogWarning(_("Skip file %s"), filename.c_str());
		}
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& dirname)  {
		wxLogMessage(_("Import subdirectory %s"), dirname.c_str());
		return wxDIR_CONTINUE;
	}
private:
	void Progress(const wxString &filename) {
		m_thread->DoStep( wxFileName(filename).GetFullName() );
	}
private:
	FbDirImportThread *m_thread;
	unsigned int m_progress;
};

void *FbDirImportThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	wxLogMessage(_("Start import directory %s"), m_dirname.c_str());

	wxDir dir(m_dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), m_dirname.c_str());
		return NULL;
	}

	{
		DoStart(0, m_dirname);
		CountTraverser counter;
		dir.Traverse(counter);
		DoStart(counter.GetCount(), m_dirname);
	}

	FolderTraverser traverser(this);
	dir.Traverse(traverser);

	DoFinish();

	wxLogMessage(_("Finish import directory %s"), m_dirname.c_str());

	return NULL;
}

void FbDirImportThread::ParseXml(const wxString &filename)
{
	wxFFileInputStream in(filename);
	if ( in.IsOk() ) {
		FbImportBook book(this, in, filename);
		if (book.IsOk()) book.Save();
	} else {
		wxLogError(_("File read error %s"), filename.c_str());
	}
}

void FbDirImportThread::ParseZip(const wxString &zipname)
{
	wxFFileInputStream in(zipname);
	if ( in.IsOk() ){
		FbImpotrZip zip(this, in, zipname);
		if (zip.IsOk()) {
			zip.Save();
			zip.Make();
		}
	} else {
		wxLogError(_("File read error %s"), zipname.c_str());
	}
}
