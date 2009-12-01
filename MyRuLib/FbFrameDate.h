#ifndef __FBFRAMEDATE_H__
#define __FBFRAMEDATE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "wx/treelistctrl.h"
#include "FbFrameBase.h"
#include "FbTreeListCtrl.h"
#include "FbBookEvent.h"

class FbFrameDate : public FbFrameBase
{
public:
	FbFrameDate(wxAuiMDIParentFrame * parent);
protected:
	virtual void CreateControls();
	virtual void UpdateBooklist();
private:
	void CreateBookInfo();
private:
	void OnGenreSelected(wxTreeEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEDATE_H__
