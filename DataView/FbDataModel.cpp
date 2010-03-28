#include "FbDataModel.h"

// -----------------------------------------------------------------------------
// class FbTitleData
// -----------------------------------------------------------------------------

wxSize FbTitleRenderer::GetSize() const
{
    const wxDataViewCtrl * view = GetView();
    int y = m_data.m_title.empty() ? wxDVC_DEFAULT_RENDERER_SIZE : view->wxWindowBase::GetTextExtent(m_data.m_title).GetHeight();
    return wxSize(GetOwner()->GetWidth(), y);
}

// -----------------------------------------------------------------------------
// class FbTitleRenderer
// -----------------------------------------------------------------------------

bool FbTitleRenderer::Render( wxRect rect, wxDC *dc, int state )
{
	int x = wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth();

	wxRect checkbox = rect;
	checkbox.SetWidth(x);

    wxLogError(wxT("Rect(%d,%d) "), rect.GetLeft(), rect.GetX());

	long flag = m_data.m_checked ? wxCONTROL_CHECKED : 0;
    wxRendererNative::Get().DrawCheckBox(GetOwner()->GetOwner(), *dc, checkbox, flag);

	RenderText(m_data.m_title, x + 2, rect, dc, state);
	return true;
}

bool FbTitleRenderer::LeftClick( wxPoint cursor, wxRect cell, wxDataViewModel *model, const wxDataViewItem &item, unsigned int col )
{
	int x = wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth();
//	if (cursor.x - cell.GetX() > x + 10) return false;

    wxLogError(wxT("Point (%d) - Rect(%d,%d) "), cursor.x, cell.GetLeft(), cell.GetX());

	m_data.m_checked = not m_data.m_checked;
    wxVariant variant;
    variant << m_data;
    model->ChangeValue(variant, item, col);
    return true;
}

bool FbTitleRenderer::SetValue( const wxVariant &value )
{
	m_data << value;
	return true;
}

