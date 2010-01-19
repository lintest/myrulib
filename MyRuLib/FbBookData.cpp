#include "FbBookData.h"
#include "FbManager.h"
#include "ZipReader.h"
#include "FbParams.h"
#include "FbDownloader.h"

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

void FbItemData::Show(wxEvtHandler * frame, bool bVertical, bool bEditable) const
{
}

void FbItemData::Open() const
{
	ZipReader reader(m_id, m_id<0);
	if ( reader.IsOK() ) {
		FbManager::OpenBook( reader.GetZip(), m_filetype);
	} else if ( m_id>0 ) {
		if ( wxMessageBox(_("Скачать книгу?"), _("Подтверждение"), wxOK | wxCANCEL) == wxOK) DoDownload();
	}
}

void FbItemData::DoDownload() const
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

