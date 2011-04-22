#ifndef __FBIMPORTWIZARD_H__
#define __FBIMPORTWIZARD_H__

#include <wx/wizard.h>

class FbImportWizard :
	public wxWizard
{
public:
    FbImportWizard() { Init(); }

	FbImportWizard(wxWindow *parent,
             int id = wxID_ANY,
             const wxString& title = wxEmptyString,
             const wxBitmap& bitmap = wxNullBitmap,
             const wxPoint& pos = wxDefaultPosition,
             long style = wxDEFAULT_DIALOG_STYLE)
    {
        Init();
        Create(parent, id, title, bitmap, pos, style);
    }

	bool Create(wxWindow *parent,
             int id = wxID_ANY,
             const wxString& title = wxEmptyString,
             const wxBitmap& bitmap = wxNullBitmap,
             const wxPoint& pos = wxDefaultPosition,
             long style = wxDEFAULT_DIALOG_STYLE);

	void Init();

	virtual ~FbImportWizard(void);

private:

};

#endif // __FBIMPORTWIZARD_H__
