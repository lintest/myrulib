#ifndef __FBEXPORTDLG_H__
#define __FBEXPORTDLG_H__

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
#include "models/FbBookList.h"
#include "controls/FbTreeView.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbWindow.h"
#include "controls/FbChoiceCtrl.h"
#include "controls/FbComboBox.h"

class FbBookPanel;

class FbConvertArray;

class FbExportTreeModel;

///////////////////////////////////////////////////////////////////////////////
/// Class FbExportDlg
///////////////////////////////////////////////////////////////////////////////
class FbExportDlg : public FbDialog
{
	public:
		FbExportDlg(wxWindow* parent, const wxString & selections, int author);
		~FbExportDlg();
		static bool Execute(wxWindow* parent, FbBookPanel * books, int author = 0);
	private:
		FbModel * CreateModel(int author, const wxString &structure);
		void FullBySequences(wxTreeItemId root, const wxString &selections, bool bUseLetter);
		void FullNoSequences(wxTreeItemId root, const wxString &selections, bool bUseLetter);
		wxTreeItemId AppendFolder(const wxTreeItemId &parent, const wxString & name);
		void ChangeFilesExt(const wxTreeItemId &parent);
		void FillFilelist(const wxTreeItemId &parent, FbConvertArray &filelist, const wxString &dir = wxEmptyString);
		void ChangeFormat(FbExportTreeModel * model);
		bool ExportBooks();
		void LoadFormats();
		wxString GetExt(int format);
	private:
		wxString m_selections;
		wxArrayString m_filenames;
		int m_author;
		FbCommonDatabase m_database;
	private:
		enum
		{
			ID_DIR_TXT = wxID_HIGHEST + 1,
			ID_STRUCT,
			ID_BOOKS,
			ID_FORMAT,
			ID_AUTHOR,
			ID_DIR,
			ID_FILE,
		};
		FbCustomCombo * m_folder;
		wxComboBox * m_struct;
		FbTreeViewCtrl * m_books;
		FbChoiceInt * m_format;
		wxCheckBox * m_checkAuthor;
		wxCheckBox * m_transDir;
		wxCheckBox * m_transFile;
	private:
        void OnSelectDir( wxCommandEvent& event );
		void OnBookCollapsing( wxTreeEvent & event );
		void OnChangeFormat( wxCommandEvent& event );
		void OnCheckAuthor( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif // __FBEXPORTDLG_H__
