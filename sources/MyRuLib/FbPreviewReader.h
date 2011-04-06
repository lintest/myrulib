#ifndef __FBPREVIEWREADER_H__
#define __FBPREVIEWREADER_H__

#include <wx/wx.h>
#include "FbParsingCtx.h"

class FbViewThread;

class FbViewData;

class FbPreviewReader: public FbParsingContext
{
	public:
		FbPreviewReader(FbViewThread & thread, FbViewData & data)
			: m_thread(thread), m_data(data) {}
	protected:
		virtual void NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
	private:
		void AppendImg(const FbStringHash &atts);
		void StartImg(const FbStringHash &atts);
	private:
		FbViewThread & m_thread;
		FbViewData & m_data;
	private:
		wxString m_isbn;
		wxString m_annt;
		wxString m_imagedata;
		wxString m_imagename;
		bool m_saveimage;
		bool m_parsebody;
		wxArrayString m_images;
};

#endif // __FBPREVIEWREADER_H__
