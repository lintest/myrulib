#include "FbImportThread.h"
#include "FbImportReader.h"
#include "FbInternetBook.h"
#include <wx/dir.h>
#include <wx/list.h>
#include <wx/wfstream.h>
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
	wxURL(MyRuLib::HomePage()).GetProtocol();
}

bool FbLibImportThread::Download(const wxString &filename)
{
	wxString addr = MyRuLib::HomePage(); addr << wxT('/') << m_lib << wxT(".zip");

	FbURL url(addr);
	wxInputStream * in = url.GetInputStream();
	if (in == NULL) {
		FbLogError(_("Download error"), addr);
		return false;
	}

	wxString msg = _("Download collection"); msg << wxT(": ") << m_lib;
	return SaveTo(*in, filename, msg);
}

bool FbLibImportThread::Extract(const wxString &filename)
{
	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	bool ok = zip.IsOk();
	if (!ok) return false;

	if (wxZipEntry * entry = zip.GetNextEntry()) {
		ok = zip.OpenEntry(*entry);
		delete entry;
	} else ok = false;
	if (!ok) return false;

	wxString msg = _("Extract file"); msg << wxT(": ") << m_lib;
	return SaveTo(zip, m_file, msg);
}

bool FbLibImportThread::SaveTo(wxInputStream &in, const wxString &filename, const wxString &msg)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	size_t pos = 0;
	size_t size = in.GetSize();
	FbProgressEvent(ID_PROGRESS_START, msg, size).Post(GetOwner());

	wxTempFileOutputStream out(filename);
	while (!IsClosed()) {
		FbProgressEvent(ID_PROGRESS_UPDATE, wxEmptyString, pos).Post(GetOwner());
		size_t count = in.Read(buf, BUFSIZE).LastRead();
		if ( count ) {
			out.Write(buf, count);
			pos += count;
		} else break;
	}
	return out.Commit();
}

bool FbLibImportThread::CreateLib()
{
	if (m_lib.IsEmpty()) return true;
	wxString tempfile = wxFileName::CreateTempFileName(wxT("fb"));
	bool ok = Download(tempfile) && Extract(tempfile);
	wxRemoveFile(tempfile);
	return ok;
}

bool FbLibImportThread::Execute()
{
	if (!CreateLib()) return false;

	if (IsClosed()) return false;

	FbProgressEvent(ID_PROGRESS_PULSE, _("Create full text search index")).Post(GetOwner());

	FbMainDatabase database;
	int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX;
	database.Open(m_file, wxEmptyString, flags);
	if (!database.IsOpen()) return false;
	database.SetText(DB_LIBRARY_DIR, m_dir);
	m_database = &database;

	if (IsClosed()) return false;

	if (m_import) {
		SetRoot(m_dir);
		FbProgressEvent(ID_PROGRESS_START, _("Processing folder:"), 1000).Post(GetOwner());
		FbCounter counter(database);
		DoParse();
		counter.Execute();
	}

	return true;
}

void * FbLibImportThread::Entry()
{
	int result = Execute() ? wxID_OK : wxID_CANCEL;
	FbCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, result).Post(GetOwner());
	return NULL;
}

