#ifndef __FBAUTHORLIST_H__
#define __FBAUTHORLIST_H__

#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/wxsqlite3.h>

class FbAuthorList: public wxListBox
{
    public:
        FbAuthorList(wxWindow* parent, wxWindowID id, long style = wxSUNKEN_BORDER)
            :wxListBox(parent, id, wxDefaultPosition, wxDefaultSize, 0, NULL, style = 0) {};
        void FillAuthorsChar(const wxChar & findLetter);
        void FillAuthorsText(const wxString & findText);
    private:
        void FillAuthors(wxSQLite3ResultSet & result);
};

#endif // __FBAUTHORLIST_H__
