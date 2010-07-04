#ifndef __FBVIEWREADER_H__
#define __FBVIEWREADER_H__

#include <wx/wx.h>
#include "ParseCtx.h"

class FbViewThread;

class FbViewData;

class FbViewReader: public ParsingContext
{
	public:
		FbViewReader(FbViewThread &thread, FbViewData &data);
		bool Load(wxInputStream& stream);
		void StartTag(wxString &name, const XML_Char **atts);
		void FinishTag(wxString &name);
		void AppendImg(const XML_Char **atts);
		void StartImg(const XML_Char **atts);
		void WriteImg(const XML_Char *s, int len);
		void FinishImg();
		void WriteText(const XML_Char *s, int len);
		void CheckLength();
	public:
		FbViewThread & m_thread;
		FbViewData & m_data;
	public:
		wxString isbn;
		wxString annt;
		wxString imagedata;
		wxString imagetype;
		wxString imagename;
		bool skipimage;
		bool parsebody;
		wxArrayString images;
};

#endif // __FBVIEWREADER_H__
