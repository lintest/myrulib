#ifndef __FBPREVIEWREADER_H__
#define __FBPREVIEWREADER_H__

#include <wx/wx.h>
#include "FbParsingCtx.h"

class FbViewThread;

class FbViewData;

WX_DECLARE_STRING_HASH_MAP( int, FbStringHashInt );

#ifdef FB_PARSE_FAXPP

class FbPreviewReaderFaxpp: public FbParsingContextFaxpp
{
	public:
		FbPreviewReaderFaxpp(FbViewThread & thread, FbViewData & data)
			: m_thread(thread), m_data(data) {}
	protected:
		virtual void NewNode(const FAXPP_Event & event, bool closed);
		virtual void EndNode(const FAXPP_Event & event);
		virtual void TxtNode(const FAXPP_Event & event);
	private:
		void AppendImg(const FAXPP_Event & event);
		void StartImg(const FAXPP_Event & event);
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

typedef FbPreviewReaderFaxpp FbPreviewReader;

#endif // FB_PARSE_FAXPP

#ifdef FB_PARSE_EXPAT

class FbPreviewReaderExpat: public FbParsingContextExpat
{
	public:
		FbPreviewReaderExpat(FbViewThread & thread, FbViewData & data)
			: m_thread(thread), m_data(data) {}
	protected:
		virtual void NewNode(const XML_Char *name, const XML_Char **atts);
		virtual void TxtNode(const XML_Char *text, int len);
		virtual void EndNode(const XML_Char* name);
	private:
		void AppendImg(const XML_Char **atts);
		void StartImg(const XML_Char **atts);
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

typedef FbPreviewReaderExpat FbPreviewReader;

#endif // FB_PARSE_EXPAT

#endif // __FBPREVIEWREADER_H__
