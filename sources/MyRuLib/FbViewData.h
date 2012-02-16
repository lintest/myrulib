#ifndef __FBVIEWDATA_H__
#define __FBVIEWDATA_H__

#include <wx/wx.h>
#include "FbViewContext.h"
#include "FbCacheBook.h"

class FbViewThread;

class FbViewData: public wxObject
{
	public:
		enum Fields {
			ANNT = 0,
			DSCR,
			ISBN,
			ICON,
			FILE,
			SEQN,
			LAST_FIELD,
		};
	public:
		static void Push(const wxString &filename, const wxImage &image);
		FbViewData(int id): m_id(id) {}
		virtual ~FbViewData();
		int GetCode() const { return m_id; }
		void SetText(size_t index, const wxString &text);
		wxString GetHTML(const FbViewContext &ctx, const FbCacheBook &book) const;
		void AddImage(FbViewThread &thread, const wxString &filename, const wxString &imagedata);
		void AddImage(FbViewThread &thread, const wxString &filename, wxInputStream &stream);
		void Reset();
	private:
		wxString GetTitle(const FbCacheBook &book) const;
		wxString GetImage(const wxString &filename) const;
		wxString GetText(size_t index) const;
		wxString GetComments(const FbViewContext &ctx, const FbCacheBook &book) const;
		static wxString HTML(const wxString &value, const bool bSingleQuotes = false, const bool bDoubleQuotes = true);
	public:
		const int m_id;
		wxString m_text[LAST_FIELD];
		wxArrayString m_images;
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbViewData, FbViewDataArray);

#endif // __FBVIEWDATA_H__
