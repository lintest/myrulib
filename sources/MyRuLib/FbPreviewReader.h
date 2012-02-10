#ifndef __FBPREVIEWREADER_H__
#define __FBPREVIEWREADER_H__

#include <wx/wx.h>
#include "FbParsingCtx.h"

class FbViewThread;

class FbViewData;

class FbPreviewReader: public FbParsingContext
{
private:
	class BookHandler : public FbHandlerXML
	{
	public:
		explicit BookHandler(FbPreviewReader &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader) {}
	protected:
		FbPreviewReader &m_reader;
	};

	class RootHandler : public BookHandler
	{
		FB2_BEGIN_KEYLIST
			Descr,
			Binary,
		FB2_END_KEYLIST
	public:
		explicit RootHandler(FbPreviewReader &reader, const wxString &name) : BookHandler(reader, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	};

	class DescrHandler : public BookHandler
	{
		FB2_BEGIN_KEYLIST
			Title,
			Publish,
		FB2_END_KEYLIST
	public:
		explicit DescrHandler(FbPreviewReader &reader, const wxString &name) : BookHandler(reader, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	};

	class ImageHandler : public BookHandler
	{
	public:
		explicit ImageHandler(FbPreviewReader &reader, const wxString &name, const wxString &file) : BookHandler(reader, name), m_file(file) {}
	private:
		const wxString m_file;
	};

public:
	FbPreviewReader(FbViewThread & thread, FbViewData & data)
		: m_thread(thread), m_data(data) {}
protected:
	FbHandlerXML * CreateHandler(const wxString &name);
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
