#include "FbDateTree.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include "FbDateTime.h"
#include "FbMasterTypes.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbDateTreeThread
//-----------------------------------------------------------------------------

void * FbDateTreeThread::Entry()
{
	FbTreeModel * model = new FbTreeModel();
	FbParentData * root = new FbParentData(*model);
	model->SetRoot(root);

	FbCommonDatabase database;
	wxString sql = wxT("SELECT id, lib_min, lib_max, lib_num, usr_min, usr_max, usr_num FROM dates ORDER BY id DESC");
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);

	FbDateYearData * year = NULL;
	FbDateMonthData * mnth = NULL;
	while (result.NextRow()) {
		int day = result.GetInt(0);
		int new_year = day / 10000;
		int new_mnth = day / 100;
		if (year == NULL || year->GetCode() != new_year) {
			year = new FbDateYearData(*model, root, new_year);
			mnth = NULL;
		}
		if (mnth == NULL || mnth->GetCode() != new_mnth) {
			mnth = new FbDateMonthData(*model, year, new_mnth);
		}
		new FbDateDayData(*model, mnth, day, result);
	}
	FbModelEvent(ID_MODEL_CREATE, model).Post(m_frame);

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbDateYearData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbDateYearData, FbParentData)

wxString FbDateYearData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0:
			return FbDateTime(m_code + 2000, 1, 1).Format(wxT("%Y"));
		default:
			return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbDateMonthData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbDateMonthData, FbParentData)

wxString FbDateMonthData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0:
			return FbDateTime(m_code / 100 + 2000, m_code % 100, 1).Format(wxT("%B %Y"));
		default:
			return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbDateDayData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbDateDayData, FbChildData)

wxString FbDateDayData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0:
			return FbDateTime(m_code).FormatDate();
		case 1:
			return Format(m_count);
		default:
			return wxEmptyString;
	}
}

FbDateDayData::FbDateDayData(FbModel & model, FbParentData * parent, int code, wxSQLite3ResultSet &result)
	: FbChildData(model, parent),
		m_code(code),
		m_count(result.GetInt(3) + result.GetInt(6)),
		m_lib_min(result.GetInt(1)),
		m_lib_max(result.GetInt(2)),
		m_usr_min(result.GetInt(4)),
		m_usr_max(result.GetInt(5))
{
}

bool FbDateDayData::operator==(const FbMasterInfo & info) const
{
	FbMasterDateInfo * data = wxDynamicCast(&info, FbMasterDateInfo);
	return data && data->GetId() == m_code;
}

