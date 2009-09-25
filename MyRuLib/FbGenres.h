#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>
#include "wx/treelistctrl.h"

class FbGenres
{
    public:
        static wxString Char(const wxString &code);
        static wxString Name(const wxString &letter);
        static wxString DecodeList(const wxString &genres);
        static void FillControl(wxTreeListCtrl * control);
};

class FbGenreData: public wxTreeItemData
{
	public:
		FbGenreData(const wxString &code): m_code(code) {};
		const wxString & GetCode() { return m_code; };
	private:
		wxString m_code;
};

#endif //FBGENRES_H

