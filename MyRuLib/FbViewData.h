#ifndef __FBVIEWDATA_H__
#define __FBVIEWDATA_H__

#include <wx/wx.h>
#include "FbViewContext.h"
#include "FbCacheBook.h"

#define MAX_IMAGE_WIDTH 200

class FbViewData: public wxObject
{
	public:
		enum Fields {
			ANNT = 0,
			DSCR,
			ISBN,
			FILE,
		};
	public:
		FbViewData(int id): m_id(id) {}
		FbViewData(const FbViewData &info);
		virtual ~FbViewData();
		int GetCode() const { return m_id; }
		void SetText(size_t index, const wxString &text);
		wxString GetHTML(const FbViewContext &ctx, const FbCacheBook &book) const;
		void AddImage(wxString &filename, wxString &imagedata, wxString &imagetype);
	private:
		wxString GetText(size_t index) const;
		wxString GetComments(const wxString md5sum, bool bEditable);
	public:
		int m_id;
		wxString m_text[FILE - 1];
		wxArrayString m_images;
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbViewData, FbViewDataArray);

#endif // __FBVIEWDATA_H__
