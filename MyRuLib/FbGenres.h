#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>
#include "wx/treelistctrl.h"

class FbGenres
{
	public:
		static wxString Char(const wxString &code);
		static wxString Name(const wxString &letter);
		static wxString Name(const int code);
		static wxString DecodeList(const wxString &genres);
		static void FillControl(wxTreeListCtrl * control);
};

class FbGenreData: public wxTreeItemData
{
	public:
		FbGenreData(const int code): m_code(code) {};
		const int GetCode() { return m_code; };
	private:
		int m_code;
};

#endif //FBGENRES_H

