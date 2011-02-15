#ifndef __FBEDITBOOK_H__
#define __FBEDITBOOK_H__

#include "FbWindow.h"
#include <wx/sizer.h>

class FbEditBookDlg : public FbDialog
{
	public:
		FbEditBookDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~FbEditBookDlg();
		static bool Execute();
	protected:
		enum
		{
			ID_TITLE_CHECK = 1000,
			ID_TITLE_EDIT,
			ID_AUTHOR_CHECK,
			ID_AUTHOR_TEXT,
			ID_AUTHOR_BTN,
			ID_SEQUENCE_CHECK,
			ID_SEQUENCE_TEXT,
			ID_SEQUENCE_BTN,
			ID_GENRE_CHECK,
			ID_GENRE_TEXT,
			ID_GENRE_BTN,
		};
	private:
		void AppenTitle(wxFlexGridSizer * sizer);
		void AppenItem(wxFlexGridSizer * sizer, wxWindowID id, const wxString &caption);
};

#endif //__FBEDITBOOK_H__
