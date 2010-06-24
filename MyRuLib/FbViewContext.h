#ifndef __FBVIEWCONTEXT_H__
#define __FBVIEWCONTEXT_H__

#include <wx/wx.h>

class FbViewContext: public wxObject
{
	public:
		FbViewContext(const FbViewContext &ctx)
			: vertical(ctx.vertical), editable(ctx.editable), filetype(ctx.filetype) {}
	public:
		bool vertical;
		bool editable;
		wxString filetype;
	private:
		DECLARE_CLASS(FbCacheBook)
};

#endif // __FBVIEWCONTEXT_H__