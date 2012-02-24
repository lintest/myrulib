#include "FbDownList.h"
#include "FbBookList.h"
#include "FbCollection.h"

IMPLEMENT_CLASS(FbDownListData, FbModelData)

IMPLEMENT_CLASS(FbDownList, FbListModel)

FbModelItem FbDownList::DoGetData(size_t row, int &level)
{ 
	level = 0; 
	FbBookListData data = FbCollection::GetDown(row);
	return FbModelItem(*this, &data);
}
