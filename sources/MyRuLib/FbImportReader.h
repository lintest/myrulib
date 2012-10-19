#ifndef __FBIMPORTREADER_H__
#define __FBIMPORTREADER_H__

#include <wx/wx.h>
#include <wx/zipstrm.h>
#include <wx/dynarray.h>
#include "FbImportCtx.h"
#include "FbParsingCtx.h"

wxString Ext(const wxString &filename);

wxString Md5(md5_context & md5);

class FbImportThread;

WX_DECLARE_STRING_HASH_MAP(wxZipEntry*, FbZipEntryMap);

WX_DECLARE_OBJARRAY(wxZipEntry*, FbZipEntryList);

class FbImportZip
	: public wxObject
{
	public:
		static int Exists(FbDatabase & database, const wxString & filename);
		FbImportZip(FbImportThread & owner, wxInputStream &in, const wxString &filename);
		int Save(bool progress);
	public:
		operator wxInputStream & () { return m_zip; }
		wxZipEntry * GetInfo(const wxString & filename);
		bool OpenEntry(wxZipEntry &entry) { return m_zip.OpenEntry(entry); }
		FbDatabase & GetDatabase() { return m_database; }
		wxString GetFilename() { return m_filename; }
		bool IsOk() { return m_ok; }
		int GetId() { return m_id; }
	private:
		void Make(bool progress);
	private:
		FbImportThread & m_owner;
		FbDatabase & m_database;
		FbZipEntryList m_list;
		FbZipEntryMap m_map;
		wxCSConv m_conv;
		wxZipInputStream m_zip;
		wxString m_filename;
		wxString m_filepath;
		wxFileOffset m_filesize;
		bool m_ok;
		int m_id;
};

class FbImportReader
	: public FbParsingContext
{
	protected:
		class BookHandler : public FbHandlerXML
		{
		public:
			explicit BookHandler(FbImportReader &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader) {}
		protected:
			FbImportReader &m_reader;
		};
	public:
		FbImportReader(): m_ok(false) {}
		virtual void OnError(wxLogLevel level, const wxString &msg, int line);
		bool IsOk() { return m_ok; };
		void Convert(FbDatabase & database);
		bool m_ok;
	public:
		wxString m_title;
		wxString m_isbn;
		wxString m_lang;
		wxString m_dscr;
		AuthorArray m_authors;
		SequenceArray m_sequences;
		wxString m_genres;
		friend class FbImportBook;
};

class FbImportBook
	: public wxObject
{
	public:
		static int Exists(FbDatabase & database, const wxString & filename);
		FbImportBook(FbImportThread & owner, wxInputStream & in, const wxString & filename);
		FbImportBook(FbImportZip & owner, wxZipEntry & entry);
		virtual ~FbImportBook();
		bool Save();
		bool IsOk() { return m_ok; };
	private:
		static wxString CalcMd5(wxInputStream& stream);
		int FindByMD5();
		bool AppendBook();
		bool AppendFile(int id_book);
	private:
		FbImportReader * m_parser;
		FbDatabase & m_database;
		wxString m_filename;
		wxString m_filepath;
		wxString m_filetype;
		wxString m_message;
		wxString m_md5sum;
		wxFileOffset m_filesize;
		int m_archive;
		bool m_ok;
};

class FbImportReaderFB2 : public FbImportReader
{
private:
	class RootHandler : public BookHandler
	{
	public:
		explicit RootHandler(FbImportReader &reader, const wxString &name) : BookHandler(reader, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	};

	class DscrHandler : public BookHandler
	{
	public:
		explicit DscrHandler(FbImportReader &reader, const wxString &name) : BookHandler(reader, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void EndNode() { m_reader.Stop(); }
	};

	class TitleHandler : public BookHandler
	{
		FB2_BEGIN_KEYLIST
			Author,
			Title,
			Annot,
			Sequence,
			Genre,
			Lang,
		FB2_END_KEYLIST
	public:
		explicit TitleHandler(FbImportReader &reader, const wxString &name) : BookHandler(reader, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void EndNode() { m_reader.Stop(); }
	};

	class AuthorHandler : public FbHandlerXML
	{
		FB2_BEGIN_KEYLIST
			Last,
			First,
			Middle,
		FB2_END_KEYLIST
	public:
		explicit AuthorHandler(FbImportReader &reader, const wxString &name);
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	private:
		AuthorItem * m_author;
	};

	class AnnotHandler : public BookHandler
	{
	public:
		explicit AnnotHandler(FbImportReader &reader, const wxString &name) : BookHandler(reader, name) {}
		explicit AnnotHandler(AnnotHandler &parent, const wxString &name) : BookHandler(parent.m_reader, name) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
	};

	class SeqnHandler : public FbHandlerXML
	{
	public:
		explicit SeqnHandler(FbImportReader &reader, const wxString &name, const FbStringHash &atts);
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	private:
		FbImportReader & m_reader;
	};

	class GenrHandler : public BookHandler
	{
	public:
		explicit GenrHandler(FbImportReader &reader, const wxString &name): BookHandler(reader, name) {}
		virtual void TxtNode(const wxString &text) { m_text << text; }
		virtual void EndNode(const wxString &name);
	private:
		wxString m_text;
	};

public:
	FbImportReaderFB2(wxInputStream & stream, bool md5 = false);

protected:
	virtual FbHandlerXML * CreateHandler(const wxString &name);
};

class FbRootReaderEPUB : public FbParsingContext
{
private:
	class RootHandler : public FbHandlerXML
	{
	public:
		explicit RootHandler(FbRootReaderEPUB &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	private:
		FbRootReaderEPUB & m_reader;
	};

	class FileHandler : public FbHandlerXML
	{
	public:
		explicit FileHandler(FbRootReaderEPUB &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
	private:
		FbRootReaderEPUB & m_reader;
	};

public:
	FbRootReaderEPUB(wxInputStream & in);
	wxString GetRoot() const { return m_rootfile; };
	bool IsOk() { return m_ok; };

protected:
	virtual FbHandlerXML * CreateHandler(const wxString &name);

private:
	wxString m_rootfile;
	bool m_ok;
};

class FbDataReaderEPUB : public FbImportReader
{
private:
	class RootHandler : public FbHandlerXML
	{
		FB2_BEGIN_KEYLIST
			Metadata,
		FB2_END_KEYLIST
	public:
		explicit RootHandler(FbDataReaderEPUB &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader) {}
		virtual FbHandlerXML *  NewNode(const wxString &name, const FbStringHash &atts);
	private:
		FbDataReaderEPUB & m_reader;
	};

	class MetaHandler : public FbHandlerXML
	{
		FB2_BEGIN_KEYLIST
			Metadata,
			Author,
			Descr,
			Title,
			Lang,
		FB2_END_KEYLIST
	public:
		explicit MetaHandler(FbDataReaderEPUB &reader, const wxString &name) : FbHandlerXML(name), m_reader(reader) {}
		virtual FbHandlerXML * NewNode(const wxString &name, const FbStringHash &atts);
		virtual void EndNode() { m_reader.Stop(); }
	private:
		FbDataReaderEPUB & m_reader;
	};

	class AuthorHandler : public FbHandlerXML
	{
	public:
		explicit AuthorHandler(FbImportReader &reader, const wxString &name);
		virtual void TxtNode(const wxString &text) { m_text << text; }
		virtual void EndNode(const wxString &name);
	private:
		AuthorItem * m_author;
		wxString m_text;
	};

public:
	FbDataReaderEPUB(wxInputStream & in, const wxString & rootfile);

protected:
	virtual FbHandlerXML * CreateHandler(const wxString &name);
};

#endif // __FBIMPORTREADER_H__
