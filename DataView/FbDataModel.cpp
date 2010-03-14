#include "FbDataModel.h"

wxSize FbTitleRenderer::GetSize() const
{
    const wxDataViewCtrl * view = GetView();
    int y = m_data.m_title.empty() ? wxDVC_DEFAULT_RENDERER_SIZE : view->wxWindowBase::GetTextExtent(m_data.m_title).GetHeight();
    return wxSize(GetOwner()->GetWidth(), y);

 //{ return wxSize(GetOwner()->GetWidth(), 20); }
}
