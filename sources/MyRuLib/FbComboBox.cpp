#include "FbComboBox.h"
#include <wx/image.h>
#include <wx/settings.h>
#include <wx/dcclient.h>

static int GetMultiplier()
{
#ifdef __WXWINCE__
    // speed up bitmap generation by using a small bitmap
    return 3;
#else
    int depth = ::wxDisplayDepth();

    if  ( depth >= 24 )
    {
        return 8;
    }
    return 6;
#endif
}

void FbLettersCombo::OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
{
	if ( item == wxNOT_FOUND )
		return;

	wxRect r(rect);
	r.Deflate(2);
	r.width -= r.height;

	wxPen pen( dc.GetTextForeground(), 1, wxSOLID );

	// Get text colour as pen colour
	dc.SetPen( pen );

	wxString text = GetString(item);
	wxString ch = text.Left(1);
	text = text.Mid(1);

	dc.DrawLabel(text, r, wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);

	wxFont font = dc.GetFont();
	font.SetWeight(wxFONTWEIGHT_BOLD);
	dc.SetFont(font);

	r = rect;
	r.Deflate(2);
	r.width = r.height * 3;

	dc.DrawLabel(ch, r, wxALIGN_CENTER);

/*

	if ( !(flags & wxODCB_PAINTING_CONTROL) )
	{
		dc.DrawText(GetString( item ),
					r.x + 3,
					(r.y + 0) + ( (r.height/2) - dc.GetCharHeight() )/2
				   );

		dc.DrawLine( r.x+5, r.y+((r.height/4)*3), r.x+r.width - 5, r.y+((r.height/4)*3) );
	}
	else
	{
		dc.DrawLine( r.x+5, r.y+r.height/2, r.x+r.width - 5, r.y+r.height/2 );
	}
*/
}

wxCoord FbLettersCombo::OnMeasureItem( size_t item ) const
{
	return m_rowHeight + 2;

	// Simply demonstrate the ability to have variable-height items
	if ( item & 1 )
		return 36;
	else
		return 24;
}

bool FbLettersCombo::SetFont(const wxFont& font)
{
    bool ok = wxOwnerDrawnComboBox::SetFont(font);
    if (ok) {
		wxClientDC dc(this);
		dc.SetFont(font);
		m_rowHeight = dc.GetCharHeight();
    }
    return ok;
}

wxBitmap FbSearchCombo::RenderButtonBitmap()
{
	wxSize sizeText = GetBestSize();
	int y  = sizeText.y - 4;
	int x = sizeText.y - 4;

    //===============================================================================
    // begin drawing code
    //===============================================================================
    // image stats

    // force width:height ratio
    if ( 14*x > y*20 )
    {
        // x is too big
        x = y*20/14;
    }
    else
    {
        // y is too big
        y = x*14/20;
    }

    // glass 11x11, top left corner
    // handle (9,9)-(13,13)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();
    int penWidth = multiplier * 1;

    penWidth = penWidth * x / 20;

    wxBitmap bitmap( multiplier*x, multiplier*y );
	wxMemoryDC mem;

	wxColour bg = GetBackgroundColour();
    wxColour fg = mem.GetTextForeground();

	bg = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

	mem.SelectObject(bitmap);

    // clear background
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int glassBase = 5 * x / 20;
    int glassFactor = 2*glassBase + 1;
    int radius = multiplier*glassFactor/2;
    mem.DrawCircle(radius,radius,radius);
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawCircle(radius,radius,radius-penWidth);

	penWidth = penWidth * 2;

    // draw handle
    int lineStart = radius + (radius-penWidth/2) * 707 / 1000; // 707 / 1000 = 0.707 = 1/sqrt(2);

    mem.SetPen( wxPen(fg) );
    mem.SetBrush( wxBrush(fg) );
    int handleCornerShift = penWidth * 707 / 1000 / 2; // 707 / 1000 = 0.707 = 1/sqrt(2);
	handleCornerShift = handleCornerShift > 0 ? handleCornerShift : 1;
    int handleBase = 4 * x / 20;
    int handleLength = 2*handleBase+1;
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*handleLength/2+handleCornerShift,multiplier*handleLength/2-handleCornerShift),
        wxPoint(multiplier*handleLength/2-handleCornerShift,multiplier*handleLength/2+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,lineStart,lineStart);

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxImage image = bitmap.ConvertToImage();
        image.Rescale(x,y);
        bitmap = wxBitmap( image );
    }

    mem.SelectObject(wxNullBitmap);

    // Finalize transparency with a mask
    wxMask *mask = new wxMask(bitmap, bg);
    bitmap.SetMask(mask);

	bitmap = bitmap.GetSubBitmap(wxRect(0,0, y,y));

    return bitmap;
}

