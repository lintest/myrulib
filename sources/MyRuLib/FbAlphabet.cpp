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
	FbSQLite3ResultSet res = database.ExecuteQuery(sql);
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

FbAlphabetCombo::FbAlphabetCombo()
	: m_rowHeight(0)
	, m_thread(NULL)
	, m_divider(-1)
{
	(m_thread = new FbAlphabetThread(this))->Execute();
}

FbAlphabetCombo::~FbAlphabetCombo()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
}

void FbAlphabetCombo::UpdateModel()
{
	FbModelItem selection = GetCurrent();
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
	(m_thread = new FbAlphabetThread(this))->Execute();
}

void FbAlphabetCombo::OnModel( FbModelEvent& event )
{
	int id = m_divider < 0 ? ID_INIT_LETTER : ID_MASTER_FIND;
	m_divider = event.GetInt();
	SetPopupControl(NULL);
	AssignModel(event.GetModel());
	FbCommandEvent(wxEVT_COMMAND_COMBOBOX_SELECTED, id).Post(this);
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

void FbAlphabetCombo::SetText(const wxString &text)
{
	FbModel * model = GetModel();
	if (model && text.Len() == 1) {
		wxString letter = Upper(text);
		size_t count = model->GetRowCount();
		for (size_t i = 1; i <= count; i++) {
			if (model->GetData(i)[0] == letter) {
				model->FindRow(i, true);
				FbComboPopup * popup = GetVListBoxComboPopup();
				if (popup) popup->SetSelection(i - 1);
				m_text.Empty();
				Refresh();
				return;
			}
		}
	}
	m_text = text;
	Refresh();
}
