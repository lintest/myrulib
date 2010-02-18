///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FbConst.h"
#include "FbCalendar.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FbCalendar, wxPanel )
	EVT_CALENDAR_SEL_CHANGED( wxID_ANY, FbCalendar::OnCalendar )
	EVT_CALENDAR_YEAR( wxID_ANY, FbCalendar::OnYearChanges )
	EVT_TREE_SEL_CHANGED( wxID_ANY, FbCalendar::OnTreeSelChanged )
END_EVENT_TABLE()

FbCalendar::FbCalendar( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_Calendar = new wxCalendarCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS );
	bSizerMain->Add( m_Calendar, 0, wxALL|wxEXPAND, 5 );

	m_DateList = new FbMasterList(this, ID_MASTER_LIST);
	m_DateList->AddColumn (_("Date"), 100, wxALIGN_LEFT);
	m_DateList->SetFocus();

	bSizerMain->Add( m_DateList, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
}

FbCalendar::~FbCalendar()
{
}
