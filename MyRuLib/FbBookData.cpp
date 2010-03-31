#include "FbBookData.h"
#include "ZipReader.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbDownloader.h"
#include "InfoCash.h"
#include <wx/mimetype.h>
#include <wx/stdpaths.h>

bool FbTempEraser::sm_erase = true;

FbTempEraser::~FbTempEraser()
{
	if (sm_erase)
		for (size_t i=0; i<filelist.GetCount(); i++)
			wxRemoveFile(filelist[i]);
}

void FbTempEraser::Add(const wxString &filename)
{
	static FbTempEraser eraser;
	eraser.filelist.Add(filename);
}

BookTreeItemData::BookTreeItemData(wxSQLite3ResultSet & res):
	m_id( res.GetInt(wxT("id"))),
	title( res.GetString(wxT("title"))),
	file_size(0), number(0), rating(0)
{
	try { file_size = res.GetInt(wxT("file_size")); } catch (...) {};
	try { file_type = res.GetString(wxT("file_type")); } catch (...) {};
	try { language = res.GetString(wxT("lang")); } catch (...) {};
	try { genres = res.GetString(wxT("genres")); } catch (...) {};
	try { number = res.GetInt(wxT("number")); } catch (...) {};
	try { int r = res.GetInt(wxT("rating")); if ( 0<=r && r<=5 ) rating = r; } catch (...) {};
}

void FbBookData::Open() const
{
	ZipReader reader(m_id, m_id<0);
	if ( reader.IsOK() ) {
		DoOpen( reader.GetZip(), reader.GetMd5() );
	} else if ( m_id>0 ) {
		if ( wxMessageBox(_("Download book file?"), _("Confirmation"), wxOK | wxCANCEL) == wxOK) DoDownload();
	}
}

void FbBookData::SaveFile(wxInputStream & in, const wxString &filepath) const
{
    FbTempEraser::Add(filepath);
    wxFileOutputStream out(filepath);
    out.Write(in);
    out.Close();
}

bool FbBookData::GetUserCommand(wxString &command) const
{
	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_filetype);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) {
	    command = result.GetString(0);
	    return true;
    }
    return false;
}

bool FbBookData::GetSystemCommand(const wxString &filepath, wxString &command) const
{
	wxFileType * ft = wxTheMimeTypesManager->GetFileTypeFromExtension(m_filetype);
	if ( ft ) {
	    bool res = ft->GetOpenCommand(&command, wxFileType::MessageParameters(filepath, wxEmptyString));
        delete ft;
        return res;
    }
	return false;
}

void FbBookData::DoOpen(wxInputStream & in, const wxString &md5sum) const
{
	wxFileName filename = md5sum;
	filename.SetPath( FbParams::GetText(FB_TEMP_DIR) );
	filename.SetExt(m_filetype);

	if ( !filename.DirExists()) filename.Mkdir(0755, wxPATH_MKDIR_FULL);

	wxString filepath = filename.GetFullPath();
	if (!filename.FileExists()) SaveFile(in, filepath);

	wxString command;
    if (GetUserCommand(command)) {
		#ifdef __WXSMW__
		ShellExecute(NULL, NULL, command, filepath, NULL, SW_SHOW);
		#else
		wxExecute(command + wxT(" \"") + filepath + wxT("\""));
		#endif
    } else if (GetSystemCommand(filepath, command)) {
		wxExecute(command);
    } else {
        wxMessageBox(_("Associated application not found") + COLON + m_filetype);
    }
}

void FbBookData::DoDownload() const
{
	if (m_id<0) return;
	wxString md5sum = FbCommonDatabase().GetMd5(m_id);
	if (md5sum.IsEmpty()) return;

	try {
		bool ok = false;
		FbLocalDatabase database;
		int folder = database.NewId(FB_NEW_DOWNLOAD);
		{
			wxString sql = wxT("UPDATE states SET download=? WHERE md5sum=?");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, folder);
			stmt.Bind(2, md5sum);
			ok = stmt.ExecuteUpdate();
		}
		if (!ok) {
			wxString sql = wxT("INSERT INTO states(download, md5sum) VALUES (?,?)");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, folder);
			stmt.Bind(2, md5sum);
			stmt.ExecuteUpdate();
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return;
	}

	FbDownloader::Push(md5sum);
	FbDownloader::Start();
}

void FbBookData::Show(wxEvtHandler * frame, bool bVertical, bool bEditable) const
{
	if (frame) {
		InfoCash::LoadIcon(m_filetype);
		InfoCash::UpdateInfo(frame, m_id, bVertical);
	}
}

void FbAuthorData::Show(wxEvtHandler * frame, bool bVertical, bool bEditable) const
{
	if (frame) (new ShowThread(frame, m_author))->Execute();
}

void * FbAuthorData::ShowThread::Entry()
{
	try {
		FbCommonDatabase database;
		wxString sql = wxT("SELECT description FROM authors WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_author);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) FbCommandEvent(fbEVT_BOOK_ACTION, ID_AUTHOR_INFO, m_author, result.GetString(0)).Post(m_frame);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

