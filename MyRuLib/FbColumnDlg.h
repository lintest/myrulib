#ifndef __FBCOLUMNDLG_H__
#define __FBCOLUMNDLG_H__

#include <wx/wx.h>
#include "FbWindow.h"

///////////////////////////////////////////////////////////////////////////////
/// Class FbColumnDlg
///////////////////////////////////////////////////////////////////////////////
class FbColumnDlg : public FbDialog
{
	public:
		FbColumnDlg( wxWindow* parent, wxWindowID id = wxID_ANY );
		static void Execute(wxWindow* parent);

	private:
		void Assign(bool write);

	private:
		enum ID {
			ID_COLUMN_LANG,
			ID_COLUMN_TYPE,
			ID_COLUMN_SYZE,
			ID_COLUMN_GENRE,
			ID_COLUMN_RATING,
			ID_REMOVE_FILES,
		};
};

#endif // __FBCOLUMNDLG_H__
