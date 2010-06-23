#include "FbDateTree.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include "FbDateTime.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbDateTreeThread
//-----------------------------------------------------------------------------

void * FbDateTreeThread::Entry()
{
	wxWindowID id = ID_MODEL_CREATE;
	try {
		FbCommonDatabase database;
		wxString sql = wxT("SELECT DISTINCT created, COUNT(DISTINCT id) FROM books GROUP BY created ORDER BY created DESC");
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		int year = 0;
		wxArrayInt items;
		while (result.NextRow()) {
			int day = result.GetInt(0);
			int new_year = day / 10000;
			if (year == 0) year = new_year;
			if (year != new_year) {
				FbArrayEvent(id, items).Post(m_frame);
				id = ID_MODEL_APPEND;
				items.Empty();
				year = new_year;
			}
			items.Add(day);
			items.Add(result.GetInt(1));
		}
		FbArrayEvent(id, items).Post(m_frame);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
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
			return FbDateTime(m_code, 1, 1).Format(wxT("%Y"));
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
			return FbDateTime(m_code / 100, m_code % 100, 1).Format(wxT("%B %Y"));
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

