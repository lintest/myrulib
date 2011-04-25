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

bool FbImportThread::OnFile(const wxString &filename, bool progress)
{
	FbAutoCommit transaction(m_database);
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

void * FbZipImportThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	size_t count = m_filelist.Count();
	wxLogMessage(_("Start import %d file(s)"), count);
	for (size_t i = 0; i < count; i++) {
		OnFile(m_filelist[i], true);
	}
	wxLogMessage(_("Finish import %d file(s)"), count);
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbDirImportThread
//-----------------------------------------------------------------------------

class CountTraverser : public wxDirTraverser
{
public:
	CountTraverser() : m_count(0) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		m_count++;
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

void * FbDirImportThread::Entry()
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
