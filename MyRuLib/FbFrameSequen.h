#ifndef __FBFRAMESEQUEN_H__
#define __FBFRAMESEQUEN_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "FbAuthorList.h"
#include "FbManager.h"

class FbFrameSequen : public FbFrameBase
{
	public:
		FbFrameSequen(wxAuiMDIParentFrame * parent);
		void FindSequence(const wxString &text);
		void OpenSequence(const int sequence, const int book);
		void SelectRandomLetter();
		void ActivateAuthors();
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
		virtual wxToolBar * CreateToolBar(long style, wxWindowID winid, const wxString& name);
	private:
		void SelectFirstAuthor(const int book = 0);
		BookTreeItemData * GetSelectedBook();
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxTextCtrl * m_FindText;
		wxString m_SequenceText;
	private:
		void OnAuthorSelected(wxTreeEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnExternal(wxCommandEvent& event);
		void OnCharEvent(wxKeyEvent& event);
		void OnEmptyAuthors(wxCommandEvent& event);
		void OnAppendAuthor(FbAuthorEvent& event);
		DECLARE_EVENT_TABLE()
	protected:
		class SequenThread: public BaseThread
		{
			public:
				SequenThread(FbFrameBase * frame, FbListMode mode, const int author)
					:BaseThread(frame, mode), m_author(author), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			protected:
				virtual void CreateTree(wxSQLite3ResultSet &result);
				virtual wxString GetSQL(const wxString & condition);
			private:
				static FbThreadSkiper sm_skiper;
				int m_author;
				int m_number;
		};
		class MasterThread: public FbThread
		{
			public:
				MasterThread(wxWindow * frame, const wxString &text, int order):  m_frame(frame), m_order(order) {};
			protected:
				virtual void * Entry();
				wxString GetOrder();
			private:
				static wxCriticalSection sm_queue;
				wxWindow * m_frame;
				int m_order;
		};
};

#endif // __FBFRAMESEQUEN_H__
