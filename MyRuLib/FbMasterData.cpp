#include "FbMasterData.h"
#include "FbMasterInfo.h"
#include "FbFrameBase.h"
#include "FbBookEvent.h"
#include "FbBookList.h"
#include "FbBookTree.h"
#include "FbAuthList.h"
#include "FbSeqnList.h"
#include "FbDateTree.h"
#include "FbConst.h"
#include "FbGenres.h"
#include "FbFrameFolder.h"
#include "FbFrameDownld.h"
#include "FbFrameSearch.h"

//-----------------------------------------------------------------------------
//  FbMasterData
//-----------------------------------------------------------------------------

int FbMasterData::sm_counter = 0;

IMPLEMENT_CLASS(FbMasterData, wxObject)

FbMasterData * FbMasterData::Create(const FbModelItem &item)
{
	{
		FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
		if (data) return new FbMasterAuthor(data->GetCode());
	} {
		FbSeqnListData * data = wxDynamicCast(&item, FbSeqnListData);
		if (data) return new FbMasterSeqname(data->GetCode());
	} {
		FbDateDayData * data = wxDynamicCast(&item, FbDateDayData);
		if (data) return new FbMasterDate(data->GetCode());
	} {
		FbGenreChildData * data = wxDynamicCast(&item, FbGenreChildData);
		if (data) return new FbMasterGenre(data->GetCode());
	} {
		FbDownListData * data = wxDynamicCast(&item, FbDownListData);
		if (data) return new FbMasterDownld(data->GetCode());
	} {
		FbFolderChildData * data = wxDynamicCast(&item, FbFolderChildData);
		if (data) return new FbMasterFolder(data->GetCode(), data->GetType());
	} {
		FbBookFindData * data = wxDynamicCast(&item, FbBookFindData);
		if (data) return new FbMasterSearch(data->GetText(), data->GetAuth());
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//  FbMasterDate
//-----------------------------------------------------------------------------

wxDateTime FbMasterDate::GetDate(int code)
{
    int m = (code / 100 % 100 - 1);
    if (m >= 12) m = 0;
    wxDateTime::Month month = wxDateTime::Month(m + wxDateTime::Jan);
	wxDateTime date = wxDateTime::Today();
    date.SetYear(code / 10000 + 2000);
    date.SetMonth(month);
    date.SetDay(code % 100);
    return date;
}

