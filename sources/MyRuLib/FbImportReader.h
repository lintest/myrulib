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
		FbImportZip(FbImportThread & owner, wxInputStream &in, const wxString &zipname);
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

class FbImportBook
	: public FbParsingContext
{
	public:
		FbImportBook(FbImportThread & owner, wxInputStream & in, const wxString & filename);
		FbImportBook(FbImportZip & owner, wxZipEntry & entry);
		bool Save();
		bool IsOk() { return m_ok; };
	protected:
		virtual void NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
	private:
		static wxString CalcMd5(wxInputStream& stream);
		int FindByMD5();
		bool AppendBook();
		bool AppendFile(int id_book);
		void Convert();
	private:
		wxString m_title;
		wxString m_isbn;
		wxString m_lang;
		AuthorArray m_authors;
		SequenceArray m_sequences;
		wxString m_genres;
		AuthorItem * m_author;
		wxString m_text;
	private:
		FbDatabase &m_database;
		wxString m_filename;
		wxString m_filepath;
		wxString m_filetype;
		wxString m_message;
		wxFileOffset m_filesize;
		int m_archive;
		bool m_parse;
		bool m_ok;
};

#endif // __FBIMPORTREADER_H__
