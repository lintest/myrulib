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
			Body,
			Descr,
			Binary,
		FB2_END_KEYLIST
	public:
		explicit RootHandler(FbPreviewReader &reader, FbViewThread & thread, FbViewData & data, const wxString &name) 
			: FbHandlerXML(name), m_reader(reader), m_thread(thread), m_data(data), m_parsebody(false) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	public:
		FbHandlerXML * NewImage(const wxString &name, const FbStringHash &atts);
		void EndImg(const wxString &file, const wxString &data);
		void AppendImg(const FbStringHash &atts);
		void SendDescr();
		void BreakBody();
	public:
		bool m_parsebody;
		wxString m_isbn;
		wxString m_annt;
	private:
		FbPreviewReader & m_reader;
		FbViewThread & m_thread;
		FbViewData & m_data;
		wxArrayString m_images;
	};

	class BookHandler : public FbHandlerXML
	{
	public:
		explicit BookHandler(RootHandler &root, const wxString &name) : FbHandlerXML(name), m_root(root) {}
	protected:
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

	class PublishHandler : public BookHandler
	{
	public:
		explicit PublishHandler(RootHandler &root, const wxString &name) : BookHandler(root, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	};

	class AnnotHandler : public BookHandler
	{
	public:
		explicit AnnotHandler(RootHandler &root, const wxString &name) : BookHandler(root, name) {}
		explicit AnnotHandler(AnnotHandler &parent, const wxString &name) : BookHandler(parent.m_root, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
	};

	class BodyHandler : public BookHandler
	{
	public:
		explicit BodyHandler(RootHandler &root, const wxString &name) : BookHandler(root, name) { root.m_parsebody = true; }
		explicit BodyHandler(BodyHandler &parent, const wxString &name) : BookHandler(parent.m_root, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
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
		virtual void EndNode(const wxString &name) { m_root.EndImg(m_file, m_data); }
	private:
		const wxString m_file;
		wxString m_data;
	};

public:
	FbPreviewReader(FbViewThread & thread, FbViewData & data)
		: m_thread(thread), m_data(data) {}

protected:
	FbHandlerXML * CreateHandler(const wxString &name);

private:
	FbViewThread & m_thread;
	FbViewData & m_data;

};

#endif // __FBPREVIEWREADER_H__
