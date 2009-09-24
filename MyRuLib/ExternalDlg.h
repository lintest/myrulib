///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ExternalDlg__
#define __ExternalDlg__

#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include "BookListCtrl.h"
#include "FbManager.h"
#include "ExpThread.h"
#include "FbConst.h"
#include "FbDatabase.h"

///////////////////////////////////////////////////////////////////////////////
/// Class ExternalDlg
///////////////////////////////////////////////////////////////////////////////
class ExternalDlg : public wxDialog
{
	public:
		ExternalDlg(wxWindow* parent, const wxString & selections, int iAuthor);
		~ExternalDlg();
		static bool Execute(wxWindow* parent, wxTreeListCtrl* books, int iAuthor = ciNoAuthor);
	private:
        static void ScanChecked(wxTreeListCtrl* bookList, const wxTreeItemId &root, wxString  &selections);
        static void ScanSelected(wxTreeListCtrl* bookList, const wxTreeItemId &root, wxString  &selections);
        void FillBooks(const wxString &selections);
        void FullBySequences(wxTreeItemId root, const wxString &selections, bool bUseLetter);
        void FullNoSequences(wxTreeItemId root, const wxString &selections, bool bUseLetter);
        void AppendBook(const wxTreeItemId &parent, BookTreeItemData &data);
        wxString GetFilename(const wxTreeItemId &parent, BookTreeItemData &data);
        wxString NormalizeDirname(const wxString &filename);
        void ChangeFilesExt(const wxTreeItemId &parent);
        void FillFilelist(const wxTreeItemId &parent, ExportFileArray &filelist, const wxString &dir = wxEmptyString);
		bool ExportBooks();
	private:
	    wxString m_selections;
        wxArrayString m_filenames;
		int m_scale;
		wxString m_ext;
		int m_author;
        FbCommonDatabase m_database;
	private:
		enum
		{
			ID_DIR_TXT = 1000,
			ID_DIR_BTN,
			ID_BOOKS,
			ID_FORMAT,
			ID_AUTHOR,
		};
		wxTextCtrl* m_textDir;
		BookListCtrl* m_books;
		wxChoice* m_choiceFormat;
		wxCheckBox* m_checkAuthor;
	private:
		void OnSelectDir( wxCommandEvent& event );
        void OnBookCollapsing( wxTreeEvent & event );
		void OnChangeFormat( wxCommandEvent& event );
		void OnCheckAuthor( wxCommandEvent& event );
        DECLARE_EVENT_TABLE()
};

#endif //__ExternalDlg__
