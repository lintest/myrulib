#include "FbBookData.h"
#include "ZipReader.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbCollection.h"
#include "FbColumns.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
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
	m_id(0), file_size(0), number(0), rating(0)
{
    Assign(res, wxT("id"), m_id);
    Assign(res, wxT("title"), title);
    Assign(res, wxT("file_size"), file_size);
    Assign(res, wxT("file_type"), file_type);
    Assign(res, wxT("lang"), language);
    Assign(res, wxT("genres"), genres);
    Assign(res, wxT("number"), number);

    Assign(res, wxT("rating"), rating);
	if ( rating<0 || 5<rating ) rating = 0;
}

void BookTreeItemData::Assign(wxSQLite3ResultSet &res, const wxString& column, int &value)
{
    for (int i=0; i<res.GetColumnCount(); i++) {
        if (res.GetColumnName(i).CmpNoCase(column)==0) {
            value = res.GetInt(i);
            return;
        }
    }
}

void BookTreeItemData::Assign(wxSQLite3ResultSet &res, const wxString& column, wxString &value)
{
    for (int i=0; i<res.GetColumnCount(); i++) {
        if (res.GetColumnName(i).CmpNoCase(column)==0) {
            value = res.GetString(i);
            return;
        }
    }
}

wxString FbBookData::GetExt() const
{
	return FbCollection::GetBook(m_id, BF_TYPE);
}

void FbBookData::Open() const
{
	ZipReader reader(m_id, m_id<0);
	if ( reader.IsOk() ) {
		DoOpen( reader.GetZip(), reader.GetMd5() );
	} else if ( m_id>0 ) {
		if ( wxMessageBox(_("Download book file?"), _("Confirmation"), wxOK | wxCANCEL) == wxOK) DoDownload();
	}
}

void FbBookData::LoadIcon() const
{
	FbCollection::LoadIcon(GetExt());
}

void FbBookData::SaveFile(wxInputStream & in, const wxString &filepath) const
{
    FbTempEraser::Add(filepath);
    wxFileOutputStream out(filepath);
    out.Write(in);
    out.Close();
}

bool FbBookData::GetUserCommand(wxSQLite3Database &database, const wxString &filetype, wxString &command) const
{
	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, filetype);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) {
	    command = result.GetString(0);
	    return !command.IsEmpty();
    }
    return false;
}

bool FbBookData::GetSystemCommand(const wxString &filepath, const wxString &filetype, wxString &command) const
{
	wxFileType * ft = wxTheMimeTypesManager->GetFileTypeFromExtension(filetype);
	if ( ft ) {
	    bool res = ft->GetOpenCommand(&command, wxFileType::MessageParameters(filepath, wxEmptyString));
        delete ft;
        return res;
    }
	return false;
}

void FbBookData::DoOpen(wxInputStream & in, const wxString &md5sum) const
{
	wxString filetype = GetExt();
	wxFileName filename = md5sum;
	filename.SetPath( FbParams::GetPath(FB_TEMP_DIR) );
	filename.SetExt(filetype);

	if ( !filename.DirExists()) filename.Mkdir(0755, wxPATH_MKDIR_FULL);

	wxString filepath = filename.GetFullPath();
	if (!filename.FileExists()) SaveFile(in, filepath);
	wxString command;
	bool ok = false;

	if (!ok) {
		FbLocalDatabase database;
		ok = GetUserCommand(database, filetype, command);
	}
	if (!ok) {
		FbCommonDatabase database;
		ok = GetUserCommand(database, filetype, command);
	}
	if (ok) {
		filepath.Prepend(wxT('"')).Append(wxT('"'));
		#ifdef __WXMSW__
		ShellExecute(NULL, NULL, command, filepath, NULL, SW_SHOW);
		#else
		if (command.Left(5) == wxT("wine ")) {
			filename.SetPath( FbParams::GetStr(FB_WINE_DIR));
			filepath = filename.GetFullPath();
		}
		command << wxT(' ') << filepath;
		wxExecute(command);
		#endif
	} else {
		#ifdef __WXGTK__
		filepath.Prepend(wxT('"')).Append(wxT('"'));
		command << wxT("xdg-open") << wxT(' ') << filepath;
		wxExecute(command);
		#else
		if (GetSystemCommand(filepath, filetype, command)) {
			wxExecute(command);
		} else {
			FbMessageBox(_("Associated application not found"), filetype);
		}
		#endif
	}
}

void FbBookData::DoDownload() const
{
	if (m_id<0) return;
	wxString md5sum = FbCommonDatabase().GetMd5(m_id);
	if (md5sum.IsEmpty()) return;

	bool ok = false;
	FbLocalDatabase database;
	int folder = - database.NewId(FB_NEW_DOWNLOAD);
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

	{
		FbMasterDownInfo info = FbMasterDownInfo::DT_WAIT;
		FbMasterEvent(ID_UPDATE_MASTER, info, m_id, true).Post();
	}

	{
		FbMasterDownInfo info = FbMasterDownInfo::DT_ERROR;
		FbMasterEvent(ID_UPDATE_MASTER, info, m_id, false).Post();
	}

	wxGetApp().StartDownload();
}

void FbBookData::Show(wxEvtHandler * frame, bool bVertical, bool bEditable) const
{
}

void FbAuthorData::Show(wxEvtHandler * frame, bool bVertical, bool bEditable) const
{
	if (frame) (new ShowThread(frame, m_author))->Execute();
}

void * FbAuthorData::ShowThread::Entry()
{
	FbCommonDatabase database;
	wxString sql = wxT("SELECT description FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_author);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) FbCommandEvent(fbEVT_BOOK_ACTION, ID_AUTHOR_INFO, m_author, result.GetString(0)).Post(m_frame);
	return NULL;
}

