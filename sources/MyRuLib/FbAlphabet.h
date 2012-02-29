#ifndef __FBALPHABET_H__
#define __FBALPHABET_H__

#include "controls/FbComboBox.h"
#include "controls/FbTreeModel.h"
#include "FbBookEvent.h"
#include "FbThread.h"

class FbAlphabetData: public FbModelData
{
	public:
		FbAlphabetData(const wxString & letter, int count = 0)
			: m_letter(letter), m_count(Format(count)) {}
		FbAlphabetData(const wxString & letter, const wxString & count)
			: m_letter(letter), m_count(count) {}
		virtual FbModelData * Clone() const
			{ return new FbAlphabetData(m_letter, m_count); }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
	private:
		wxString m_letter;
		wxString m_count;
		DECLARE_CLASS(FbAlphabetData);
};

class FbAlphabetThread: public FbThread
{
	public:
		FbAlphabetThread(wxEvtHandler * owner)
			: FbThread(wxTHREAD_JOINABLE), m_owner(owner) {}

	protected:
		virtual void * Entry();

	private:
		wxEvtHandler * m_owner;
};

class FbAlphabetCombo : public FbComboBox
{
	public:
		FbAlphabetCombo();

		virtual ~FbAlphabetCombo();

		void UpdateModel();

		virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int index, FbModelItem item, int flags ) const;

		virtual wxCoord OnMeasureItem( size_t item ) const;

		virtual wxCoord OnMeasureItemWidth( size_t WXUNUSED(item) ) const { return -1; }

		virtual bool SetFont(const wxFont& font);

		void SetText(const wxString &text = wxEmptyString);

	private:
		int m_rowHeight;
		FbAlphabetThread * m_thread;
		wxString m_text;
		int m_divider;

	private:
		void OnModel( FbModelEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif // __FBALPHABET_H__
