#ifndef __FBAUTHORDLG_H__
#define __FBAUTHORDLG_H__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FbAuthorDlg
///////////////////////////////////////////////////////////////////////////////
class FbAuthorDlg : public wxDialog
{
	public:
		static bool Append();
		static bool Modify(int id);
	public:
		FbAuthorDlg( const wxString& title = wxEmptyString, int id = 0 );
		~FbAuthorDlg();
	protected:
		enum
		{
			ID_LAST_NAME = 1000,
			ID_FIRST_NAME,
			ID_MIDDLE_NAME,
		};
	private:
		bool Load(int id);
		int m_id;
	private:
		void AppenName(wxFlexGridSizer * parent, wxWindowID id, const wxString &caption);
		void SetValue(wxWindowID id, const wxString &text);
};

#endif //__FBAUTHORDLG_H__
