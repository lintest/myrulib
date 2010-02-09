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
	file_type( res.GetString(wxT("file_type"))),
	file_size( res.GetInt(wxT("file_size"))),
	number( res.GetInt(wxT("number"))),
	genres( res.GetString(wxT("genres"))),
	rating(0),
	language( res.GetString(wxT("lang")))
{
	int r = res.GetInt(wxT("rating"));
	if ( r>=0 && r<=5 ) rating = r;
}

void FbBookData::Open() const
{
	ZipReader reader(m_id, m_id<0);
	if ( reader.IsOK() ) {
		DoOpen( reader.GetZip(), reader.GetMd5() );
	} else if ( m_id>0 ) {
		if ( wxMessageBox(_("Скачать книгу?"), _("Подтверждение"), wxOK | wxCANCEL) == wxOK) DoDownload();
	}
}

void FbBookData::DoOpen(wxInputStream & in, const wxString &md5sum) const
{
	wxFileName file_name = md5sum;
	file_name.SetPath( FbParams::GetText(FB_TEMP_DIR) );
	file_name.SetExt(m_filetype);

	if ( !file_name.DirExists()) file_name.Mkdir(0777, wxPATH_MKDIR_FULL);

	wxString file_path = file_name.GetFullPath();
	FbTempEraser::Add(file_path);
	wxFileOutputStream out(file_path);
	out.Write(in);

	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_filetype);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if ( result.NextRow() ) {
		wxString command = result.GetString(0);
		#if defined(__WIN32__)
		ShellExecute(NULL, NULL, command, file_path, NULL, SW_SHOW);
		#else
		wxExecute(command + wxT(" ") + file_path);
		#endif
		return;
	}

	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(m_filetype);
	if ( ft ) {
		wxString cmd;
		if (ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(file_path, wxEmptyString))) {
			wxExecute(cmd);
			return;
		}
		delete ft;
	}

	wxString msg = _("Не найдено приложение для просмотра файлов типа ") + m_filetype;
	wxMessageBox(msg);
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

