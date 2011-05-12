#ifndef __FBALPHABET_H__
#define __FBALPHABET_H__

#include <wx/dcmemory.h>
#include <wx/combo.h>
#include <wx/odcombo.h>
#include "FbBookEvent.h"
#include "FbThread.h"

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

class FbAlphabetCombo : public wxOwnerDrawnComboBox
{
	public:
		FbAlphabetCombo()
			: m_rowHeight(0), m_thread(this), m_divider(-1) { m_thread.Execute(); }

		virtual ~FbAlphabetCombo()
			{ m_thread.Close(); m_thread.Wait(); }

		virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const;

		virtual wxCoord OnMeasureItem( size_t item ) const;

		virtual wxCoord OnMeasureItemWidth( size_t WXUNUSED(item) ) const
			{ return -1; }

		virtual bool SetFont(const wxFont& font);

		void SetText(const wxString &text = wxEmptyString)
			{ m_text = text; Refresh(); }

	private:
		int m_rowHeight;
		FbAlphabetThread m_thread;
		wxString m_text;
		int m_divider;

	private:
		void OnLetters(FbLettersEvent &event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBALPHABET_H__
