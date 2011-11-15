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
		bool IsOk() { return m_ok; };
	private:
		void Make(bool progress);
		bool OpenEntry(wxZipEntry &entry) { return m_zip.OpenEntry(entry); };
		wxZipEntry * GetInfo(const wxString & filename);
	private:
		FbImportThread & m_owner;
		FbDatabase &m_database;
		FbZipEntryList m_list;
		FbZipEntryMap m_map;
		wxCSConv m_conv;
		wxZipInputStream m_zip;
		wxString m_filename;
		wxString m_filepath;
		wxFileOffset m_filesize;
		friend class FbImportBook;
		bool m_ok;
		int m_id;
};

class FbImportParser
	: public FbParsingContext
{
	protected:
		void Convert(FbDatabase & database);
	protected:
		wxString m_title;
		wxString m_isbn;
		wxString m_lang;
		AuthorArray m_authors;
		SequenceArray m_sequences;
		wxString m_genres;
		AuthorItem * m_author;
		wxString m_text;
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
		FbImportParser * m_parser;
		FbDatabase & m_database;
		wxString m_filename;
		wxString m_filepath;
		wxString m_filetype;
		wxString m_message;
		wxString m_md5sum;
		wxFileOffset m_filesize;
		int m_archive;
		bool m_parse;
		bool m_ok;
};

class FbImportParserFB2
	: public FbImportParser
{
	protected:
		virtual void NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
};

class FbRootReaderEPUB
	: public FbParsingContext
{
	public:
		FbRootReaderEPUB(wxInputStream & in);
		wxString GetRoot() const { return m_rootfile; };
	protected:
		virtual void NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text) {};
		virtual void EndNode(const wxString &name);
	private:
		wxZipInputStream m_zip;
		wxString m_rootfile;
		bool m_ok;
};

#endif // __FBIMPORTREADER_H__
