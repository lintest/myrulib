#ifndef __FBVIEWCONTEXT_H__
#define __FBVIEWCONTEXT_H__

#include <wx/wx.h>

class FbViewContext: public wxObject
{
	public:
		FbViewContext()
			: vertical(false), editable(false) {}
		FbViewContext(const FbViewContext &ctx)
			: vertical(ctx.vertical), editable(ctx.editable) {}
		FbViewContext & operator =(const FbViewContext &ctx)
			{ vertical = ctx.vertical; editable = ctx.editable; return *this; }
	public:
		bool vertical;
		bool editable;
	private:
		DECLARE_CLASS(FbViewContext)
};

#endif // __FBVIEWCONTEXT_H__
