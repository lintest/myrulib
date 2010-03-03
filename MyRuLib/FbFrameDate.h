#ifndef __FBFRAMEDATE_H__
#define __FBFRAMEDATE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"

class FbFrameDate : public FbFrameBase
{
	public:
		FbFrameDate(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Calendar"); };
		void ActivateAuthors();
		void Localize(bool bUpdateMenu);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
	private:
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void SelectFirstAuthor(const int book = 0);
		BookTreeItemData * GetSelectedBook();
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxTextCtrl * m_FindText;
		wxStaticText * m_FindInfo;
		wxString m_SequenceText;
		int m_SequenceCode;
	private:
		void OnMasterSelected(wxTreeEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
	protected:
		class MasterThread: public FbThread
		{
			public:
				MasterThread(wxEvtHandler * frame): m_frame(frame) {};
			protected:
				virtual void * Entry();
			private:
				static wxCriticalSection sm_queue;
				wxEvtHandler * m_frame;
		};
        class MasterList: public FbMasterList
        {
            public:
                MasterList(wxWindow *parent, wxWindowID id, long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER)
                    : FbMasterList(parent, id, style) {};
            private:
                wxTreeItemId m_owner;
                wxTreeItemId m_parent;
                int m_year;
                int m_month;
            private:
                void OnAppendMaster(FbMasterEvent& event);
                DECLARE_EVENT_TABLE()
        };
};

#endif // __FBFRAMEDATE_H__
