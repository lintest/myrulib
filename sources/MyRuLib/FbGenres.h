#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>
#include "FbTreeModel.h"

class FbGenres
{
	public:
		static wxString Char(const wxString &code);
		static wxString Name(const wxString &letter);
		static wxString DecodeList(const wxString &genres);
		static FbModel * CreateModel();
	private:
		enum ID {
			ID_CHAR,
			ID_NAME,
			ID_FILL,
		};
		static void Do(ID id, const void * value, void * result);
};

#endif //FBGENRES_H

