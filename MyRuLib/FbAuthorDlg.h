#ifndef __FBAUTHORDLG_H__
#define __FBAUTHORDLG_H__

#include "FbWindow.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FbAuthorDlg
///////////////////////////////////////////////////////////////////////////////
class FbAuthorDlg : public FbDialog
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
