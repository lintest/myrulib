#ifndef __FBPREVIEWREADER_H__
#define __FBPREVIEWREADER_H__

#include <wx/wx.h>
#include "FbParsingCtx.h"

class FbViewThread;

class FbViewData;

class FbPreviewReader: public FbParsingContext
{
	public:
		FbPreviewReader(FbViewThread & thread, FbViewData & data);
		virtual ~FbPreviewReader();
	protected:
		virtual bool OnProcessEvent(const FAXPP_Event & event);
	private:
		void NewNode(const FAXPP_Event & event);
		void EndNode(const FAXPP_Event & event);
		void TxtNode(const FAXPP_Event & event);
		void AppendImg(const FAXPP_Event & event);
		void StartImg(const FAXPP_Event & event);
		void Stop() { m_stop = true; }
	private:
		FbViewThread & m_thread;
		FbViewData & m_data;
		bool m_stop;
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
