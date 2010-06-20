#include "FbMasterInfo.h"
#include "FbBookList.h"
#include "FbBookTree.h"
#include "FbBookPanel.h"

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterInfo, wxObject)

//-----------------------------------------------------------------------------
//  FbMasterAuthorInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterAuthorInfo, FbMasterInfo)

FbThread * FbMasterAuthorInfo::GetThread(FbBookPanel * owner) const
{
	switch (GetMode()) {
		case FB2_MODE_TREE: return new FbBookTreeThread(owner, m_id);
		case FB2_MODE_LIST: return new FbBookListThread(owner, m_id);
	}
}

