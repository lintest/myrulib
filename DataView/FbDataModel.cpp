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
	int x = wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth() + 2;
	m_offsets[m_data.m_level] = rect.GetLeft();

	wxRect checkbox = rect;
	checkbox.SetWidth(x);

	long flag = m_data.m_checked ? wxCONTROL_CHECKED : 0;
    wxRendererNative::Get().DrawCheckBox(GetOwner()->GetOwner(), *dc, checkbox, flag);

	RenderText(m_data.m_title, x, rect, dc, state);

	return true;
}

bool FbTitleRenderer::LeftClick( wxPoint cursor, wxRect cell, wxDataViewModel *model, const wxDataViewItem &item, unsigned int col )
{
	#ifdef __WXWIN__
    wxPoint point = GetOwner()->GetOwner()->CalcUnscrolledPosition(cursor);
    int left  = m_offsets[m_data.m_level] - 2;
    int right = left + wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth() + 2;
    #endif
	#ifdef __WXGTK__
    wxPoint point = cursor;
	int left  = 0;
    int right = wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth() + 2;
    #endif

	if ((left <= point.x) && (point.x <= right)) {
        m_data.m_checked = not m_data.m_checked;
        wxVariant variant;
        variant << m_data;
        model->ChangeValue(variant, item, col);
        return true;
	}
    return wxDataViewCustomRenderer::LeftClick(cursor, cell, model, item, col);
}

bool FbTitleRenderer::SetValue( const wxVariant &value )
{
	m_data << value;
	return true;
}

bool FbTitleRenderer::Activate( wxRect cell, wxDataViewModel *model, const wxDataViewItem & item, unsigned int col)
{
    wxLogMessage(wxT("Activate"));
	return true;
}
