#ifndef __FBPREVIEWREADER_H__
#define __FBPREVIEWREADER_H__

#include <wx/wx.h>
#include "FbParsingCtx.h"

class FbViewThread;

class FbViewData;

class FbPreviewReader: public FbParsingContext
{
private:
	class RootHandler : public FbHandlerXML
	{
		FB2_BEGIN_KEYLIST
			Descr,
			Binary,
		FB2_END_KEYLIST
	public:
		explicit RootHandler(FbPreviewReader &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader), m_parsebody(false) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		FbHandlerXML * NewImage(const wxString &name, const FbStringHash &atts);
		FbPreviewReader & Reader() { return m_reader; }
		void AppendImg(const FbStringHash &atts);
		void SendDescr();
	public:
		wxString m_isbn;
		wxString m_annt;
	private:
		FbPreviewReader &m_reader;
		wxArrayString m_images;
		bool m_parsebody;
	};

	class BookHandler : public FbHandlerXML
	{
	public:
		explicit BookHandler(RootHandler &root, const wxString &name) : FbHandlerXML(name), m_root(root) {}
	protected:
		FbPreviewReader & Reader() { return m_root.Reader(); }
		RootHandler &m_root;
	};

	class DescrHandler : public BookHandler
	{
		FB2_BEGIN_KEYLIST
			Title,
			Publish,
		FB2_END_KEYLIST
	public:
		explicit DescrHandler(RootHandler &root, const wxString &name) : BookHandler(root, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void EndNode(const wxString &name);
	};

	class TitleHandler : public BookHandler
	{
		FB2_BEGIN_KEYLIST
			Annot,
			Cover,
		FB2_END_KEYLIST
	public:
		explicit TitleHandler(RootHandler &root, const wxString &name) : BookHandler(root, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	};

	class AnnotHandler : public BookHandler
	{
	public:
		explicit AnnotHandler(RootHandler &root, const wxString &name, wxString &text) : BookHandler(root, name), m_text(text) {}
		explicit AnnotHandler(AnnotHandler &parent, const wxString &name) : BookHandler(parent.m_root, name), m_text(parent.m_text) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text) { m_text << text; }
		virtual void EndNode(const wxString &name);
	private:
		wxString & m_text;
	};

	class CoverHandler : public BookHandler
	{
	public:
		explicit CoverHandler(RootHandler &root, const wxString &name) : BookHandler(root, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	};

	class ImageHandler : public BookHandler
	{
	public:
		explicit ImageHandler(RootHandler &root, const wxString &name, const wxString &file) : BookHandler(root, name), m_file(file) {}
		virtual void TxtNode(const wxString &text) { m_data << text; }
		virtual void EndNode(const wxString &name);
	private:
		const wxString m_file;
		wxString m_data;
	};

public:
	FbPreviewReader(FbViewThread & thread, FbViewData & data)
		: m_thread(thread), m_data(data) {}
	void AppendImage(const wxString &file, const wxString &data);
	FbViewThread & m_thread;
	FbViewData & m_data;

protected:
	FbHandlerXML * CreateHandler(const wxString &name);

};

#endif // __FBPREVIEWREADER_H__
