#ifndef __FBFRAMESEARCH_H__
#define __FBFRAMESEARCH_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "FbFrameBase.h"

class FbFrameSearch : public FbFrameBase
{
	public:
		static void Execute(wxAuiMDIParentFrame * parent, const wxString &title, const wxString &author);
		FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString & title = wxEmptyString);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
	private:
		void CreateBookInfo();
		void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
	private:
		wxTextCtrl * m_textTitle;
		wxTextCtrl * m_textAuthor;
		wxString m_author;
		wxString m_title;
	private:
		void OnFoundNothing(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
	protected:
		class SearchThread: public BaseThread
		{
			public:
				SearchThread(FbFrameBase * frame, FbListMode mode, const wxString &title, const wxString &author)
					: BaseThread(frame, mode), m_title(title), m_author(author) {};
				virtual void *Entry();
			private:
				wxString m_title;
				wxString m_author;
		};
};

#endif // __FBFRAMESEARCH_H__
