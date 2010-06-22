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

class FbBookFindData: public FbModelData
{
	public:
		FbBookFindData(const wxString &text, const wxString &auth)
			: m_text(text), m_auth(auth) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return col == 1 ? m_auth : m_text; }
		wxString GetText() const 
			{ return m_text; }
		wxString GetAuth() const 
			{ return m_auth; }
		virtual FbModelData * Clone() const
			{ return new FbBookFindData(m_text, m_auth); }
	private:
		wxString m_text;
		wxString m_auth;
		DECLARE_CLASS(FbAuthListData);
};

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
