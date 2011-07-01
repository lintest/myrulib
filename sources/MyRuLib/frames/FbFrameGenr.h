#ifndef __FBFRAMEGENR_H__
#define __FBFRAMEGENR_H__

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

class FbFrameGenr : public FbFrameBase
{
	public:
		FbFrameGenr(wxAuiNotebook * parent, bool select = false);
		virtual wxString GetTitle() const { return _("Genres"); };
	private:
		void CreateBookInfo();
		void CreateColumns();
	private:
		void OnBooksCount(FbCountEvent& event);
		void OnModel( FbModelEvent & event );
		DECLARE_CLASS(FbFrameGenr)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEGENR_H__
