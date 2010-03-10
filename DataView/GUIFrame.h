///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUIFrame__
#define __GUIFrame__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/frame.h>

#include "FbBookModel.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class GUIFrame
///////////////////////////////////////////////////////////////////////////////
class GUIFrame : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
		void _wxFB_OnQuit( wxCommandEvent& event ){ OnQuit( event ); }
		void _wxFB_OnAbout( wxCommandEvent& event ){ OnAbout( event ); }
		void _wxFB_OnOpen( wxCommandEvent& event ){ OnOpen( event ); }

		void OnActivated(wxDataViewEvent& event);

	protected:
		enum
		{
			idMenuQuit = 1000,
			idMenuOpen,
			idMenuAbout,
			idDataView,
		};

		wxStatusBar* m_statusbar;
		wxDataViewCtrl* m_dataview;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ){ event.Skip(); }

	public:
		GUIFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("wxWidgets Application Template"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 481,466 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~GUIFrame();

};

#endif //__GUIFrame__
