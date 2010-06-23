#ifndef __FBFRAMESEARCH_H__
#define __FBFRAMESEARCH_H__

#include "FbFrameBase.h"

class FbFrameSearch : public FbFrameBase
{
	public:
		static void Execute(wxAuiMDIParentFrame * parent, const wxString &title, const wxString &author);
		FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString &title, const wxString &author);
		virtual wxString GetTitle() const;
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
		virtual void CreateColumns() {}
	private:
		void CreateBookInfo();
		void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
	private:
		wxTextCtrl * m_textTitle;
		wxTextCtrl * m_textAuthor;
		wxString m_title;
		wxString m_author;
	private:
		void OnFoundNothing(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameSearch)
};

#endif // __FBFRAMESEARCH_H__
