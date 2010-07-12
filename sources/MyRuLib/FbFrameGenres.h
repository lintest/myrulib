#ifndef __FBFRAMEGENRES_H__
#define __FBFRAMEGENRES_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "FbFrameBase.h"
#include "FbBookEvent.h"

class FbFrameGenres : public FbFrameBase
{
	public:
		FbFrameGenres(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Genres"); };
	protected:
		virtual void CreateControls();
	private:
		void CreateBookInfo();
		void CreateColumns();
	private:
		void OnBooksCount(FbCountEvent& event);
		void OnModel( FbModelEvent & event );
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameGenres)
};

#endif // __FBFRAMEGENRES_H__
