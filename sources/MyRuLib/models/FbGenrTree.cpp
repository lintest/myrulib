#include "FbGenrTree.h"
#include "FbBookEvent.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbMasterTypes.h"

//-----------------------------------------------------------------------------
//  FbGenrParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbGenrParentData, FbParentData)

//-----------------------------------------------------------------------------
//  FbGenrChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbGenrChildData, FbChildData)

wxString FbGenrChildData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0:
			return m_name;
		case 1:
			return m_count ? Format(m_count) : wxString();
		default:
			return wxEmptyString ;
	}
}

bool FbGenrChildData::operator==(const FbMasterInfo & info) const
{
	FbMasterGenrInfo * data = wxDynamicCast(&info, FbMasterGenrInfo);
	return data && data->GetId() == m_code;
}

//-----------------------------------------------------------------------------
//  FbGenrListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbGenrListData, FbModelData)

FbGenrListData::FbGenrListData(wxSQLite3ResultSet &result)
	: m_code(result.GetString(0)), m_count(result.GetInt(1))
{
}

//-----------------------------------------------------------------------------
//  FbGenrChildData
//-----------------------------------------------------------------------------

void * FbGenrListThread::Entry()
{
	FbCommonDatabase database;
	database.JoinThread(this);

	wxSQLite3ResultSet result = database.ExecuteQuery(m_sql);
	if (!result.IsOk()) return NULL;
	if (IsClosed()) return NULL;

	FbModel * model = new FbListStore;
	while (result.NextRow()) model->Append(new FbGenrListData(result));

	if (IsClosed()) {
		delete model;
	} else {
		FbModelEvent(ID_MODEL_CREATE, model).Post(m_frame);
	}

	return NULL;
}

