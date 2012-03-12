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
	FbFrameDatabase database(this, m_counter);
	wxString sql = wxT("SELECT DISTINCT created FROM books ORDER BY 1 DESC");
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (result.IsOk()) MakeModel(result);
	CreateCounter(database, m_sql);
	return NULL;
}

void FbDateTreeThread::MakeModel(wxSQLite3ResultSet &result)
{
	bool ok = true;

	FbTreeModel * model = new FbDateTreeModel;
	FbParentData * root = new FbParentData(*model);
	model->SetRoot(root);

	FbDateYearData * year = NULL;
	FbDateMonthData * mnth = NULL;
	while (result.NextRow()) {
		if (IsClosed()) { ok = false; break; }
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
		new FbDateDayData(*model, mnth, day);
	}

	if (ok) {
		FbModelEvent(ID_MODEL_CREATE, model).Post(m_frame);
	} else {
		delete model;
	}
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
		case 1: {
			FbDateTreeModel * master = wxDynamicCast(&model, FbDateTreeModel);
			if (master) {
				int count = master->GetCount(m_code);
				if (count != wxNOT_FOUND) return FbCollection::Format(count);
			}
			return wxEmptyString;
		}
		default:
			return wxEmptyString;
	}
}

FbDateDayData::FbDateDayData(FbModel & model, FbParentData * parent, int code)
	: FbChildData(model, parent), m_code(code), m_count(0)
{
}

bool FbDateDayData::operator==(const FbMasterInfo & info) const
{
	FbMasterDateInfo * data = wxDynamicCast(&info, FbMasterDateInfo);
	return data && data->GetId() == m_code;
}

//-----------------------------------------------------------------------------
//  FbDateTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbDateTreeModel, FbTreeModel)

int FbDateTreeModel::GetCount(int code)
{
	if (m_counter.count(code)) return m_counter[code];
	int count = FbFrameThread::GetCount(m_database, code);
	if (count == wxNOT_FOUND) return wxNOT_FOUND;
	m_counter[code] = count;
	return count;
}

void FbDateTreeModel::SetCounter(const wxString & filename)
{
	if (!filename.IsEmpty()) m_database.Open(filename);
	m_counter.clear();
}
