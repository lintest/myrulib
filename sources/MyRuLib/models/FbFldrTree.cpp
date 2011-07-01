#include "FbFldrTree.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbFolderParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFolderParentData, FbParentData)

//-----------------------------------------------------------------------------
//  FbFolderChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFolderChildData, FbChildData)

//-----------------------------------------------------------------------------
//  FbCommChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCommChildData, FbChildData)

FbCommChildData::FbCommChildData(FbModel & model, FbParentData * parent)
	: FbChildData(model, parent), m_name(_("Comments")) {}

//-----------------------------------------------------------------------------
//  FbRateChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbRateChildData, FbChildData)

FbRateChildData::FbRateChildData(FbModel & model, FbParentData * parent, int code)
	: FbChildData(model, parent), m_code(code), m_name(GetRatingText(code)) {}
