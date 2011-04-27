#include "FbImportThread.h"
#include "FbImportReader.h"
#include "FbInternetBook.h"
#include <wx/dir.h>
#include <wx/list.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbDateTime.h"
#include "FbGenres.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "MyRuLibApp.h"
#include "polarssl/md5.h"
#include "wx/base64.h"
#include "controls/FbURL.h"

//-----------------------------------------------------------------------------
//  FbImportThread
//-----------------------------------------------------------------------------

FbImportThread::FbImportThread(wxEvtHandler * owner, wxThreadKind kind)
	: FbProgressThread(owner, kind),
		m_database(NULL),
		m_basepath(wxGetApp().GetLibPath()),
		m_fullpath(FbParams::GetInt(FB_SAVE_FULLPATH))
{
}

void FbImportThread::SetRoot(const wxString & dir)
{
	m_basepath = dir;
}

void * FbImportThread::Entry()
{
	SetRoot(wxGetApp().GetLibPath());

	FbCommonDatabase database;
	m_database = &database;

	FbCounter counter(database);
	DoParse();
	counter.Execute();

	return NULL;
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

bool FbImportThread::OnFile(const wxString &filename, bool progress)
{
	FbAutoCommit transaction(*m_database);
	wxFFileInputStream in(filename);
	if (Ext(filename) == wxT("zip")) {
		return FbImportZip(*this, in, filename).Save(progress);
	} else {
		return FbImportBook(*this, in, filename).Save();
	}
}

//-----------------------------------------------------------------------------
//  FbZipImportThread
//-----------------------------------------------------------------------------

void FbZipImportThread::DoParse()
{
	SetInfo(_("Processing file:"));
	wxCriticalSectionLocker enter(sm_queue);

	size_t count = m_filelist.Count();
	wxLogMessage(_("Start import %d file(s)"), count);
	for (size_t i = 0; i < count; i++) {
		if (IsClosed()) break;
		OnFile(m_filelist[i], true);
	}
	wxLogMessage(_("Finish import %d file(s)"), count);
}

//-----------------------------------------------------------------------------
//  FbDirImportThread
//-----------------------------------------------------------------------------

class FbImportCounter : public wxDirTraverser
{
public:
	FbImportCounter() : m_count(0) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		m_count++;
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) {
		return wxDIR_CONTINUE;
	}
	unsigned int GetCount() {
		return m_count;
	}
private:
	unsigned int m_count;
};

class FbImportTraverser : public wxDirTraverser
{
public:
	FbImportTraverser(FbDirImportThread* thread) : m_thread(thread), m_progress(0) { };

	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		if (m_thread->IsClosed()) return wxDIR_STOP;
		m_thread->DoStep( wxFileName(filename).GetFullName() );
		m_thread->OnFile(filename, false);
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		if (m_thread->IsClosed()) return wxDIR_STOP;
		wxLogMessage(_("Import subdirectory %s"), dirname.c_str());
		return wxDIR_CONTINUE;
	}

private:
	FbDirImportThread *m_thread;
	unsigned int m_progress;
};

void FbDirImportThread::DoParse()
{
	SetInfo(_("Processing folder:"));
	wxCriticalSectionLocker enter(sm_queue);

	wxLogMessage(_("Start import directory %s"), m_dirname.c_str());

	wxDir dir(m_dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), m_dirname.c_str());
		return;
	}

	{
		DoStart(m_dirname);
		FbImportCounter counter;
		dir.Traverse(counter);
		DoStart(m_dirname, counter.GetCount());
	}

	FbImportTraverser traverser(this);
	dir.Traverse(traverser);

	DoFinish();

	wxLogMessage(_("Finish import directory %s"), m_dirname.c_str());
}

//-----------------------------------------------------------------------------
//  FbLibImportThread
//-----------------------------------------------------------------------------

FbLibImportThread::FbLibImportThread(wxEvtHandler * owner, const wxString &file, const wxString &dir, const wxString &lib, bool import)
	: FbDirImportThread(owner, dir, wxTHREAD_JOINABLE), m_file(file), m_dir(dir), m_lib(lib), m_import(import)
{
	wxURL(strHomePage).GetProtocol().SetTimeout(FbParams::GetInt(FB_WEB_TIMEOUT));
}

bool FbLibImportThread::Download()
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	wxString addr = strHomePage; addr << wxT('/') << m_lib << wxT(".zip");

	wxString tempfile = wxFileName::CreateTempFileName(wxT("fb"));

	{
		FbURL url(addr);

		wxInputStream * in = url.GetInputStream();
		if (in == NULL) {
			FbLogError(_("Download error"), addr);
			return false;
		}

		wxFileOutputStream out(tempfile);

		size_t pos = 0;
		size_t size = in->GetSize();
		wxString msg = _("Download collection"); msg << wxT(": ") << m_lib;
		FbProgressEvent(ID_PROGRESS_START, msg, size).Post(GetOwner());

		while (true) {
			if (IsClosed()) break;
			FbProgressEvent(ID_PROGRESS_UPDATE, wxEmptyString, pos).Post(GetOwner());
			size_t count = in->Read(buf, BUFSIZE).LastRead();
			if ( count ) {
				out.Write(buf, count);
				pos += count;
			} else break;
		}
		out.Close();
		if (pos != size) {
			wxRemoveFile(tempfile);
			return false;
		}
	}

	if (IsClosed()) {
		wxRemoveFile(tempfile);
		return false;
	};

	{
		wxFFileInputStream in(tempfile);
		wxZipInputStream zip(in);

		bool ok = zip.IsOk();
		if (!ok) return false;

		if (wxZipEntry * entry = zip.GetNextEntry()) {
			ok = zip.OpenEntry(*entry);
			delete entry;
		} else ok = false;

		if (!ok) return false;

		wxFileOutputStream out(m_file);
		out.Write(zip);
		return out.IsOk();
	}

	wxRemoveFile(tempfile);
}

void * FbLibImportThread::Entry()
{
	if (!m_lib.IsEmpty()) {
		bool ok = Download();
		if (!ok) return NULL;
	}

	if (IsClosed()) return NULL;

	FbProgressEvent(ID_PROGRESS_PULSE, _("Create full text search index")).Post(GetOwner());

	FbMainDatabase database;
	int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX;
	database.Open(m_file, wxEmptyString, flags);
	if (!database.IsOpen()) return NULL;
	m_database = &database;

	{
		wxString sql = wxT("INSERT OR REPLACE INTO params(id, text) VALUES(?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, DB_LIBRARY_DIR);
		stmt.Bind(2, m_dir);
		stmt.ExecuteUpdate();
	}	

	SetRoot(m_dir);

	if (m_import) {
		FbProgressEvent(ID_PROGRESS_START, _("Processing folder:"), 1000).Post(GetOwner());
		FbCounter counter(database);
		DoParse();
		counter.Execute();
	}

	return NULL;
}

void FbLibImportThread::OnExit()
{
	FbCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK).Post(GetOwner());
}

