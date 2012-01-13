#include "FbBookData.h"
#include "FbBookEvent.h"
#include "FbFileReader.h"
#include "FbParams.h"
#include "FbCollection.h"
#include "FbColumns.h"
#include "FbConst.h"
#include "FbSmartPtr.h"
#include "MyRuLibApp.h"
#include "FbMainFrame.h"
#include "FbSmartPtr.h"
#include "frames/FbCoolReader.h"
#include <wx/mimetype.h>
#include <wx/stdpaths.h>

wxString FbBookData::GetExt() const
{
	return FbCollection::GetBook(m_id, BF_TYPE);
}

void FbBookData::LoadIcon() const
{
	FbCollection::LoadIcon(GetExt());
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

