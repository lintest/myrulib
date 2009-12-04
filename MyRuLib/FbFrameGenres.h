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
#include "wx/treelistctrl.h"
#include "FbFrameBase.h"
#include "FbTreeListCtrl.h"
#include "FbBookEvent.h"

class FbFrameGenres : public FbFrameBase
{
	public:
		FbFrameGenres(wxAuiMDIParentFrame * parent);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
	private:
		void CreateBookInfo();
	private:
		void OnGenreSelected(wxTreeEvent & event);
		DECLARE_EVENT_TABLE()
	protected:
		class GenresThread: public BaseThread
		{
			public:
				GenresThread(FbFrameGenres * frame, FbListMode mode, const int code)
					:BaseThread(frame, mode), m_code(code), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			private:
				static FbThreadSkiper sm_skiper;
				int m_code;
				int m_number;
		};
};

#endif // __FBFRAMEGENRES_H__
