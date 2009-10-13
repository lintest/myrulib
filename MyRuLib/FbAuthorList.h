#ifndef __FBAUTHORLIST_H__
#define __FBAUTHORLIST_H__

#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/wxsqlite3.h>
#include "FbTreeListCtrl.h"

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
        void FillAuthorsChar(const wxChar & findLetter);
        void FillAuthorsText(const wxString & findText);
        void FillAuthorsCode(const int code);
    private:
        void FillAuthors(wxSQLite3ResultSet & result);
};

#endif // __FBAUTHORLIST_H__
