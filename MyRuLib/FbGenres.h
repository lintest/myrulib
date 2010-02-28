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
	private:
		enum ID {
			ID_CHAR,
			ID_NAME,
			ID_DECODE,
			ID_FILL,
		};
		static void Do(ID id, const void * value, void * result);
};

#endif //FBGENRES_H

