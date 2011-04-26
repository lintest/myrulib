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

void * FbImportThread::Entry()
{
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
		m_thread->DoStep( wxFileName(filename).GetFullName() );
		m_thread->OnFile(filename, false);
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& dirname)  
	{
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
	: FbDirImportThread(owner, dir, wxTHREAD_JOINABLE), m_file(file), m_lib(lib), m_import(import) 
{
	wxURL(strHomePage).GetProtocol().SetTimeout(FbParams::GetInt(FB_WEB_TIMEOUT));
}

void * FbLibImportThread::Entry()
{
	if (!m_lib.IsEmpty()) {
		wxString addr = strHomePage; addr << wxT('/') << m_lib << wxT(".zip");
		bool ok = FbInternetBook::Download(GetOwner(), addr, m_file);
		if (!ok) return NULL;
	}

	if (IsClosed()) return NULL;

	FbMainDatabase database;
	int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX;
	database.Open(m_file, wxEmptyString, flags);
	if (!database.IsOpen()) return NULL;
	m_database = &database;

	if (m_import) {
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

