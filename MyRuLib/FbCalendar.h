///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FbCalendar__
#define __FbCalendar__

#include <wx/intl.h>
#include <wx/calctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include "FbTreeListCtrl.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FbCalendar
///////////////////////////////////////////////////////////////////////////////
class FbCalendar : public wxPanel
{
	public:
		FbCalendar( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
		~FbCalendar();

	public:
		wxCalendarCtrl* m_Calendar;
		FbTreeListCtrl* m_DateList;

	protected:
		void OnCalendar( wxCalendarEvent& event ){ event.Skip(); }
		void OnYearChanges( wxCalendarEvent& event ){ event.Skip(); }
		void OnTreeSelChanged( wxTreeEvent& event ){ event.Skip(); }
		DECLARE_EVENT_TABLE()
};

#endif //__FbCalendar__
