#ifndef __FBAUTHORLIST_H__
#define __FBAUTHORLIST_H__

#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/wxsqlite3.h>
#include "FbTreeListCtrl.h"
#include "FbUpdateThread.h"

class FbAuthorData: public wxTreeItemData
{
	public:
		FbAuthorData(const int id): m_id(id) {};
		const int GetId() { return m_id; };
	private:
		int m_id;
};

class FbAuthorList: public FbTreeListCtrl
{
	public:
		FbAuthorList(wxWindow* parent, wxWindowID id);
		FbAuthorData * GetSelected();
	private:
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		bool SelectItem(const wxTreeItemId &root, int id);
	private:
		void OnAuthorAppend(wxCommandEvent& event);
		void OnAuthorModify(wxCommandEvent& event);
		void OnAuthorDelete(wxCommandEvent& event);
		void OnAuthorReplace(wxCommandEvent& event);
		void OnContextMenu(wxTreeEvent& event);
		DECLARE_EVENT_TABLE();
	private:
		class DeleteThread: public FbUpdateThread
		{
			public:
				DeleteThread(int author): FbUpdateThread(wxEmptyString), m_author(author) {};
			protected:
				virtual void * Entry();
			private:
				int m_author;
		};
};

#endif // __FBAUTHORLIST_H__
