#ifndef __FBPREVIEWREADER_H__
#define __FBPREVIEWREADER_H__

#include <wx/wx.h>
#include "FbParsingCtx.h"

class FbViewThread;

class FbViewData;

class FbPreviewReader: public wxObject
{
	public:
		FbPreviewReader(wxInputStream & stream, FbViewThread & thread, FbViewData & data);
		virtual ~FbPreviewReader();
		bool Parse();
		int Read(char * buffer, int len);
	private:
		bool ProcessEvent(const FAXPP_Event * event);
		void NewNode(const wxString &name, int level);
		void EndNode(const wxString &name, int level);
		void TxtNode(const wxString &name, const wxString &text);
		void AppendImg();
		void StartImg();
		void Stop() { m_stop = true; }
	private:
		wxInputStream & m_stream;
		FAXPP_Parser * m_parser;
		FbParsingContext m_context;
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
