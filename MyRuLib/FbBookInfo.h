#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>

#define MAX_IMAGE_WIDTH 200

class FbBookInfo: public wxObject
{
	public:
		enum Fields {
			ANNT = 0,
			DSCR,
			ISBN,
			FILE,
		};
	public:
		FbBookInfo(int id): m_id(id) {};
		virtual ~FbBookInfo();
		void AddImage(wxString &filename, wxString &imagedata, wxString &imagetype);
		wxString GetHTML(const wxString &md5sum, bool bVertical, bool bEditable, const wxString &filetype);
	private:
		wxString GetComments(const wxString md5sum, bool bEditable);
	public:
		int m_id;
		wxString m_text[FILE - 1];
		wxArrayString m_images;
};

WX_DECLARE_OBJARRAY(FbBookInfo, InfoNodeArray);

#endif __FBBOOKEVENT_H__