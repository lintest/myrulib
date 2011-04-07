#ifndef __FBIMPORTREADER_H__
#define __FBIMPORTREADER_H__

#include <wx/wx.h>
#include <wx/zipstrm.h>
#include "ImpContext.h"
#include "FbParsingCtx.h"

class FbImportThread;

class FbImpotrZip;

class FbImportBook: public ParsingContext
{
	public:
		FbImportBook(FbImportThread * owner, wxInputStream &in, const wxString &filename);
		FbImportBook(FbImpotrZip * owner, wxZipEntry *entry);
		bool Load(wxInputStream& stream);
		bool Save();
		bool IsOk() { return m_ok; };
	protected:
		virtual void NewNode(const wxString &name, const FbStringHash &atts);
		virtual void TxtNode(const wxString &text);
		virtual void EndNode(const wxString &name);
	public:
		wxString title;
		wxString isbn;
		wxString lang;
		AuthorArray authors;
		SequenceArray sequences;
		wxString genres;
		AuthorItem * author;
		wxString text;
	private:
		static wxString CalcMd5(wxInputStream& stream);
		int FindByMD5();
		int FindBySize();
		bool AppendBook();
		bool AppendFile(int id_book);
		void Convert();
	private:
		FbDatabase &m_database;
		wxString m_md5sum;
		wxString m_filename;
		wxString m_filepath;
		wxString m_message;
		wxFileOffset m_filesize;
		int m_archive;
		bool m_ok;
};

#endif // __FBIMPORTREADER_H__
