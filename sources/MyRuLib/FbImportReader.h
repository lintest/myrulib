#ifndef __FBIMPORTREADER_H__
#define __FBIMPORTREADER_H__

#include <wx/wx.h>
#include <wx/zipstrm.h>
#include "FbImportCtx.h"
#include "FbParsingCtx.h"

class FbImportThread;

class FbImpotrZip;

class FbImportBook: public FbParsingContext
{
	public:
		FbImportBook(FbImportThread * owner, wxInputStream &in, const wxString &filename);
		FbImportBook(FbImpotrZip * owner, wxZipEntry *entry);
		bool Save();
		bool IsOk() { return m_ok; };
	protected:
		virtual void NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
	private:
		static wxString CalcMd5(wxInputStream& stream);
		int FindByMD5();
		int FindBySize();
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
