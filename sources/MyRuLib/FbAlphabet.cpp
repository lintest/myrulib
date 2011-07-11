#include "FbAlphabet.h"
#include "FbDatabase.h"
#include "FbCollection.h"
#include "FbConst.h"
#include "FbParams.h"

//-----------------------------------------------------------------------------
//  FbAlphabetData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAlphabetData, FbModelData)

wxString FbAlphabetData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case  0: return m_letter;
		case  1: return m_count;
		default: return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbAlphabetThread
//-----------------------------------------------------------------------------

void * FbAlphabetThread::Entry()
{
	int level = 0;
	int count = 0;
	int divider = 0;

	wxString engA = (wxChar) 0x0041;
	wxString rusA = (wxChar) 0x0410;
	wxString last = FbParams(FB_LAST_LETTER).Str().Left(1);

	FbListStore * model = new FbListStore;

	wxString sql = wxT("SELECT DISTINCT letter, count(id) FROM authors GROUP BY letter ORDER BY letter");
	FbCommonDatabase database;
	database.JoinThread(this);
	wxSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		wxString letter = Upper(res.GetString(0).Left(1));
		if (letter.IsEmpty()) continue;
		int num = res.GetInt(1);
		model->Append(new FbAlphabetData(letter, num));
		switch (level) {
			case 0:                            { model->GoLastRow(); level++; };
			case 1: if ( letter.Cmp(engA)>=0 ) { model->GoLastRow(); level++; };
			case 2: if ( letter.Cmp(rusA)>=0 ) { model->GoLastRow(); level++; };
			case 3: if ( letter == last      ) { model->GoLastRow(); level++; };
		}
		if (divider == 0 && letter.Cmp(rusA) >= 0) divider = model->GetRowCount();
		count += num;
	}
	model->Insert(new FbAlphabetData(wxT('*'), count), 0);
	FbModelEvent(ID_MASTER_FIND, model, divider).Post(m_owner);

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbAlphabetCombo
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbAlphabetCombo, FbComboBox)
	EVT_FB_MODEL(ID_MASTER_FIND, FbAlphabetCombo::OnModel)
END_EVENT_TABLE()

void FbAlphabetCombo::OnModel( FbModelEvent& event )
{
	m_divider = event.GetInt();
	AssignModel(event.GetModel());
	SetPopupControl(NULL);
	FbCommandEvent(wxEVT_COMMAND_COMBOBOX_SELECTED, ID_INIT_LETTER).Post(this);
}

void FbAlphabetCombo::OnDrawItem( wxDC& dc, const wxRect& rect, int index, FbModelItem item, int flags ) const
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

	wxRect r(rect);
	r.Deflate(2);
	r.width -= r.height;

	wxPen pen( dc.GetTextForeground(), 1, wxDOT );
	dc.SetPen( pen );

	dc.DrawLabel(item[1], r, wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);

	wxFont font = dc.GetFont();
	font.SetWeight(wxFONTWEIGHT_BOLD);
	dc.SetFont(font);

	r = rect;
	r.Deflate(2);
	r.width = r.height * 3;

	dc.DrawLabel(item[0], r, wxALIGN_CENTER);

	if (index == m_divider && !(flags & wxODCB_PAINTING_CONTROL)) {
		dc.DrawLine( rect.GetTopLeft(), rect.GetTopRight() );
	}
}

wxCoord FbAlphabetCombo::OnMeasureItem( size_t item ) const
{
	return m_rowHeight;
}

bool FbAlphabetCombo::SetFont(const wxFont& font)
{
	bool ok = FbComboBox::SetFont(font);
	if (ok) {
		wxClientDC dc(this);
		dc.SetFont(font);
		m_rowHeight = dc.GetCharHeight() + 4;
	}
	return ok;
}
/*
void FbAlphabetCombo::OnLetters(FbLettersEvent &event)
{
	Append(event.GetLetters());
	SetSelection(event.GetPosition());
	m_divider = event.GetDivider();
	FbCommandEvent(wxEVT_COMMAND_COMBOBOX_SELECTED, ID_INIT_LETTER).Post(this);
}
*/