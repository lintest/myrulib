#include "FbAlphabet.h"
#include "FbDatabase.h"
#include "FbCollection.h"
#include "FbConst.h"
#include "FbParams.h"

//-----------------------------------------------------------------------------
//  FbAlphabetThread
//-----------------------------------------------------------------------------

void * FbAlphabetThread::Entry()
{
	int divider = wxNOT_FOUND;

	int position = 0;
	wxString engA = (wxChar) 0x0041;
	wxString rusA = (wxChar) 0x0410;
	wxString last = FbParams(FB_LAST_LETTER).Str().Left(1);

	int pos = 0;
	int level = 0;
	int count = 0;
	
	wxArrayString items;
	wxString sql = wxT("SELECT distinct letter, count(id) FROM authors GROUP BY letter ORDER BY letter");
	FbCommonDatabase database;
	database.JoinThread(this);
	wxSQLite3ResultSet set = database.ExecuteQuery(sql);
	while (set.NextRow()) {
		pos++;
		wxString text = set.GetString(0).Left(1);
		if (text.IsEmpty()) continue;
		switch (level) {
			case 0: {
				position = pos; level++;
			} // continue;
			case 1: if ( text.Cmp(engA)>=0 ) {
				position = pos; level++;
			} // continue;
			case 2: if ( text.Cmp(rusA)>=0 ) {
				position = pos; level++;
			} // continue;
			case 3: if ( text == last ) {
				position = pos; level++;
			} // contine;
		}
		if (divider == wxNOT_FOUND && text.Cmp(rusA)>=0) divider = pos;
		int num = set.GetInt(1);
		text << FbCollection::Format(num);
		items.Add(text);
		count += num;
	}

	wxString text = wxT('*');
	text << FbCollection::Format(count);
	items.Insert(text, 0);

	FbLettersEvent(wxID_ANY, items, position, divider).Post(m_owner);

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbAlphabetCombo
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbAlphabetCombo, wxOwnerDrawnComboBox)
	EVT_FB_LETTERS(wxID_ANY, FbAlphabetCombo::OnLetters)
END_EVENT_TABLE()

void FbAlphabetCombo::OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
{
	if ((flags & wxODCB_PAINTING_CONTROL) && !m_text.IsEmpty()) {
		wxRect r(rect);
		r.Deflate(2);

		wxFont font = dc.GetFont();
		font.SetWeight(wxFONTWEIGHT_BOLD);
		dc.SetFont(font);

		dc.DrawLabel(m_text, r, wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL);
		return;
	}

	if ( item == wxNOT_FOUND ) return;

	wxRect r(rect);
	r.Deflate(2);
	r.width -= r.height;

	wxPen pen( dc.GetTextForeground(), 1, wxDOT );
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

	if (item == m_divider && !(flags & wxODCB_PAINTING_CONTROL)) {
		dc.DrawLine( rect.GetTopLeft(), rect.GetTopRight() );
	}
}

wxCoord FbAlphabetCombo::OnMeasureItem( size_t item ) const
{
	return m_rowHeight;
}

bool FbAlphabetCombo::SetFont(const wxFont& font)
{
	bool ok = wxOwnerDrawnComboBox::SetFont(font);
	if (ok) {
		wxClientDC dc(this);
		dc.SetFont(font);
		m_rowHeight = dc.GetCharHeight() + 4;
	}
	return ok;
}

void FbAlphabetCombo::OnLetters(FbLettersEvent &event)
{
	Append(event.GetLetters());
	SetSelection(event.GetPosition());
	m_divider = event.GetDivider();
	FbCommandEvent(wxEVT_COMMAND_COMBOBOX_SELECTED, ID_INIT_LETTER).Post(this);
}
