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

FbImportThread::FbImportThread(wxEvtHandler * owner, long flags)
	: FbProgressThread(owner, wxTHREAD_JOINABLE),
		m_database(NULL),
		m_fullpath(FbParams(FB_SAVE_FULLPATH)),
		m_flags(flags)
{
}

void * FbImportThread::Entry()
{
	int result = Execute() ? wxID_OK : wxID_CANCEL;
	FbCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, result).Post(GetOwner());
	return NULL;
}

bool FbImportThread::Execute()
{
	SetRoot(wxGetApp().GetLibPath());

	FbCommonDatabase database;
	database.JoinThread(this);
	m_database = &database;

	if (IsClosed()) return false;

	DoParse(HasFlag(fbIMP_ONLY_NEW));

	return true;
}

void FbImportThread::SetRoot(const wxString & dir)
{
	m_basepath = dir;
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

bool FbImportThread::OnFile(const wxString &filename, bool progress, bool only_new)
{
	FbAutoCommit transaction(*m_database);
	wxFFileInputStream in(filename);
	if (Ext(filename) == wxT("zip")) {
		if (only_new && FbImportZip::Exists(*m_database, GetRelative(filename))) return false;
		return FbImportZip(*this, in, filename).Save(progress);
	} else {
		if (only_new && FbImportBook::Exists(*m_database, GetRelative(filename))) return false;
		return FbImportBook(*this, in, filename).Save();
	}
}

//-----------------------------------------------------------------------------
//  FbZipImportThread
//-----------------------------------------------------------------------------

void FbZipImportThread::DoParse(bool only_new)
{
	size_t count = m_filelist.Count();
	wxLogMessage(_("Start import %d file(s)"), count);
	for (size_t i = 0; i < count; i++) {
		if (IsClosed()) break;
		OnFile(m_filelist[i], true, only_new);
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
	FbImportTraverser(FbDirImportThread * thread, bool only_new)
		: m_thread(thread), m_progress(0), m_only_new(only_new) {}

	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		if (m_thread->IsClosed()) return wxDIR_STOP;
		m_thread->DoStep( wxFileName(filename).GetFullName() );
		m_thread->OnFile(filename, false, m_only_new);
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		if (m_thread->IsClosed()) return wxDIR_STOP;
		wxLogMessage(_("Import subdirectory %s"), dirname.c_str());
		return wxDIR_CONTINUE;
	}

private:
	FbDirImportThread * m_thread;
	unsigned int m_progress;
	bool m_only_new;
};

void FbDirImportThread::DoParse(bool only_new)
{
	wxLogMessage(_("Start import directory %s"), m_dirname.c_str());

	wxDir dir(m_dirname);
	if ( !dir.IsOpened() ) {
		wxLogError(_("Can't open directory %s"), m_dirname.c_str());
		return;
	}

	{
		wxString msg = _("Processing folder:");
		msg << wxT(' ') << m_dirname;
		DoPulse(msg);
		FbImportCounter counter;
		dir.Traverse(counter);
		DoStart(msg, counter.GetCount());
	}

	FbImportTraverser traverser(this, only_new);
	dir.Traverse(traverser);

	DoFinish();

	wxLogMessage(_("Finish import directory %s"), m_dirname.c_str());
}

//-----------------------------------------------------------------------------
//  FbLibImportThread
//-----------------------------------------------------------------------------

FbLibImportThread::FbLibImportThread(wxEvtHandler * owner, const wxString &file, const wxString &dir, const wxString &lib, long flags)
	: FbDirImportThread(owner, dir, flags), m_file(file), m_dir(dir), m_lib(lib)
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

	DoPulse(_("Create full text search index"));

	FbMainDatabase database;
	int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX;
	database.Open(m_file, wxEmptyString, flags);
	if (!database.IsOpen()) return false;
	database.JoinThread(this);
	database.CreateFullText(false, this);
	database.SetText(DB_LIBRARY_DIR, m_dir);
	m_database = &database;

	if (IsClosed()) return false;

	if (HasFlag(fbIMP_IMPORT)) {
		SetRoot(m_dir);
		DoParse(HasFlag(fbIMP_ONLY_NEW));
	}

	return true;
}

