#define fbMAX_FILENAME_LENGTH 100

/*
static wxString wxFileName::GetForbiddenChars( wxPathFormat format = wxPATH_NATIVE )
Returns the characters that can't be used in filenames and directory names for the specified format.
*/

#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/arrimpl.cpp>
#include "ExternalDlg.h"
#include "FbParams.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbBookPanel.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ExternalDlg, wxDialog )
	EVT_BUTTON( ID_DIR_BTN, ExternalDlg::OnSelectDir )
	EVT_TREE_ITEM_COLLAPSING( ID_BOOKS, ExternalDlg::OnBookCollapsing )
	EVT_CHOICE( wxID_ANY, ExternalDlg::OnChangeFormat )
	EVT_CHECKBOX( ID_AUTHOR, ExternalDlg::OnCheckAuthor )
END_EVENT_TABLE()

wxString ExternalDlg::Normalize(const wxString &filename, bool translit)
{
    const wxString forbidden = wxT("*?\\/:\"<>|");

	wxString oldname = filename;

	bool space = false;
	wxString newname;
	size_t length = oldname.Length();
	for (size_t i = 0; i < length; i++) {
		wxChar ch = oldname[i];
		if (0 <= ch && ch < 0x20) continue;
		if (0 <= ch && ch < 0x20) continue;
		if (space && ch == 0x20) continue;
		if (forbidden.Find(ch) != wxNOT_FOUND) continue;
		if (ch == (wxChar)0x0401) ch = (wxChar)0x0415;
		if (ch == (wxChar)0x0451) ch = (wxChar)0x0435;
		space = ch == 0x20;
		newname << ch;
	}
	newname = newname.Trim(false).Trim(true).Left(fbMAX_FILENAME_LENGTH);

	wxEncodingConverter ec;
	ec.Init(wxFONTENCODING_UNICODE, wxFONTENCODING_CP1251, wxCONVERT_SUBSTITUTE);
	newname = ec.Convert(newname);

	if (translit) {
		wxChar * transchar[32] = {
			wxT("a"), wxT("b"), wxT("v"), wxT("g"), wxT("d"), wxT("e"), wxT("zh"), wxT("z"), 
			wxT("i"), wxT("j"), wxT("k"), wxT("l"), wxT("m"), wxT("n"), wxT("o"), wxT("p"), 
			wxT("r"), wxT("s"), wxT("t"), wxT("u"), wxT("f"), wxT("h"), wxT("c"), wxT("ch"), 
			wxT("sh"), wxT("shh"), wxT("'"), wxT("y"), wxT("'"), wxT("e"), wxT("yu"), wxT("ya"),
		};
		oldname = newname;
		newname.Empty();
		size_t length = oldname.Length();
		for (size_t i = 0; i < length; i++) {
			unsigned char ch = oldname[i] % 0x100;
			if (0xC0 <= ch && ch <= 0xDF) {
				newname << wxString(transchar[ch - 0xC0]).Upper();
			} else if (0xE0 <= ch && ch <= 0xFF) {
				newname << wxString(transchar[ch - 0xE0]);
			} else newname << wxChar(ch);
		}
	}

	ec.Init(wxFONTENCODING_CP1251, wxFONTENCODING_UNICODE, wxCONVERT_SUBSTITUTE);
	newname = ec.Convert(newname);

	while (newname.Left(1) == wxT(".")) newname = newname.Mid(1);
	while (newname.Right(1) == wxT(".")) newname = newname.Mid(0, newname.Len()-1);

	return newname;
}

wxString ExternalDlg::GetFilename(const wxTreeItemId &parent, BookTreeItemData &data)
{
	wxString newname = data.title;
	wxString filepath = wxT("/");
	wxTreeItemId node = parent;
	while (node.IsOk()) {
		filepath = m_books->GetItemText(node) + wxT("/") + filepath;
		node = m_books->GetItemParent(node);
	}

	newname = Normalize(newname, FbParams::GetValue(FB_TRANSLIT_FILE));

	if (data.number) newname = wxString::Format(wxT("%d_%s"), data.number, newname.c_str());

	wxFileName result = wxString::Format(wxT("%s.%s%s"), newname.c_str(), data.file_type.c_str(), m_ext.c_str());

	for (int i=1; true; i++) {
		result.Normalize(wxPATH_NORM_DOTS);
		wxFileName searchname = filepath + result.GetFullName();
		searchname.Normalize(wxPATH_NORM_CASE);
		if (m_filenames.Index(searchname.GetFullPath()) == wxNOT_FOUND) {
			m_filenames.Add(searchname.GetFullPath());
			break;
		}
		result = wxString::Format(wxT("%s(%d).%s%s"), newname.c_str(), i, data.file_type.c_str(), m_ext.c_str());
	}

	return result.GetFullName();
}

ExternalDlg::ExternalDlg( wxWindow* parent, const wxString & selections, int iAuthor) :
	FbDialog( parent, wxID_ANY, _("Export to external storage"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
	m_selections(selections),
	m_author(iAuthor),
	m_checkAuthor(NULL)
{
	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerDir;
	bSizerDir = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * m_staticTextDir = new wxStaticText( this, wxID_ANY, _("Destination folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDir->Wrap( -1 );
	bSizerDir->Add( m_staticTextDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_textDir = new wxTextCtrl( this, ID_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textDir->SetMinSize( wxSize( 300,-1 ) );

	bSizerDir->Add( m_textDir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBitmapButton * m_bpButtonDir = new wxBitmapButton( this, ID_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerDir->Add( m_bpButtonDir, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerDir, 0, wxEXPAND, 5 );

	if (iAuthor != ciNoAuthor) {
		m_checkAuthor = new wxCheckBox( this, ID_AUTHOR, _("Use Author (without co-Authors)"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizerMain->Add( m_checkAuthor, 0, wxALL, 5 );
		m_checkAuthor->SetValue(1);
	}

	long treeStyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_books = new FbTreeListCtrl( this, ID_BOOKS, treeStyle );
	m_books->SetMinSize( wxSize( -1,250 ) );
	m_books->AddColumn (_("File name"), 4, wxALIGN_LEFT);
	m_books->AddColumn (_("Size, Kb"), 1, wxALIGN_RIGHT);

	bSizerMain->Add( m_books, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFormat;
	bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * m_staticTextFormat = new wxStaticText( this, wxID_ANY, _("Export As..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFormat->Wrap( -1 );
	bSizerFormat->Add( m_staticTextFormat, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_format = new FbChoiceFormat( this, ID_FORMAT, wxDefaultPosition, wxDefaultSize);
	bSizerFormat->Add( m_format, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerFormat, 0, wxEXPAND, 5 );

	m_textDir->SetValue( FbParams::GetText(FB_EXTERNAL_DIR) );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	SetAffirmativeId(wxID_OK);
	SetEscapeId(wxID_CANCEL);

	LoadFormats();
}

ExternalDlg::~ExternalDlg()
{
}

void ExternalDlg::LoadFormats()
{
	wxString filename = _("filename");
	int format = FbParams::GetValue(FB_FILE_FORMAT);
	m_format->Append(filename << wxT(".fb2"), 0);
	m_format->Append(filename + wxT(".zip"), -1);
	m_format->Append(filename + wxT(".gz"), -2);
	m_format->SetSelection(format == -1 ? 1 : 0);

	wxString sql = wxT("SELECT id, name FROM script ORDER BY id");

	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while ( result.NextRow() ) {
		int code = result.GetInt(0);
		wxString name = _("filename"); name << wxT('.') << result.GetString(1);
		int index = m_format->Append(name, code);
		if (code == format) m_format->SetSelection(index);
	}
}

void ExternalDlg::FillBooks(const wxString &selections)
{
	int format = m_format->GetCurrentData();
	m_scale = format < 0 ? 43 : 100; // при сжатии средний коэффициент 0.43
	switch (format) {
		case -1: m_ext = wxT(".zip"); break;
		case -2: m_ext = wxT(".gz"); break;
		default: m_ext.Empty();
	}

	m_filenames.Empty();

	FbTreeListUpdater updater(m_books);

	m_books->DeleteRoot();
	wxTreeItemId root = m_books->AddRoot(wxT("root"));
	m_books->SetItemBold(root, true);

	int iFormat = FbParams::GetValue(FB_FOLDER_FORMAT);
	switch (iFormat) {
		case 0: FullBySequences(root, selections, true);  break;
		case 1: FullNoSequences(root, selections, true);  break;
		case 2: FullBySequences(root, selections, false); break;
		case 3: FullNoSequences(root, selections, false); break;
	}
}

void ExternalDlg::FullBySequences(wxTreeItemId root, const wxString &selections, bool bUseLetter)
{
	wxString sql = wxT("\
		SELECT DISTINCT (CASE WHEN bookseq.id_seq IS NULL THEN 1 WHEN bookseq.id_seq=0 THEN 1 ELSE 0 END) AS key, \
			books.id, books.title, books.file_size, books.file_type, books.file_name, books.id_author, authors.letter, authors.full_name, bookseq.id_seq, sequences.value AS sequence, bookseq.number\
		FROM books \
			LEFT JOIN authors ON authors.id=books.id_author \
			LEFT JOIN bookseq ON bookseq.id_book=books.id \
			LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
		WHERE books.id IN (%s) %s \
		ORDER BY key, authors.letter, authors.full_name, books.id_author, sequences.value, bookseq.number, books.title \
	");
	wxString filter;
	if ( m_checkAuthor && m_checkAuthor->GetValue() )
		filter = wxString::Format(wxT("AND (books.id_author=%d)"), m_author);
	sql = wxString::Format(sql, selections.c_str(), filter.c_str());

	wxArrayInt books;
	wxString thisLeter, thisAuthor, thisSequence;
	wxTreeItemId itemLetter, itemAuthor, itemSequence, itemParent;

	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
	while (result.NextRow()) {
		BookTreeItemData data(result);
		if ( books.Index(data.GetId()) != wxNOT_FOUND ) continue;
		wxString nextAuthor = result.GetString(wxT("full_name"));
		wxString nextSequence = result.GetString(wxT("sequence"));
		if (bUseLetter) {
			wxString nextLetter = result.GetString(wxT("letter"));
			if (thisLeter!= nextLetter || !itemLetter.IsOk()) {
				thisLeter = nextLetter;
				itemAuthor = 0L;
				itemSequence = 0L;
				itemLetter = AppendFolder(root, thisLeter);
				m_books->SetItemBold(itemLetter, true);
			}
		} else {
			itemLetter = root;
		}
		if (thisAuthor != nextAuthor || !itemAuthor.IsOk()) {
			thisAuthor = nextAuthor;
			itemSequence = 0L;
			itemParent = itemAuthor = AppendFolder(itemLetter, thisAuthor);
			m_books->SetItemBold(itemAuthor, true);
		}
		if (thisSequence != nextSequence || !itemSequence.IsOk()) {
			thisSequence = nextSequence;
			if (result.GetInt(wxT("id_seq"))) {
				wxString seqname = thisSequence.IsEmpty() ? (wxString)_("(Misc.)") : thisSequence;
				itemParent = itemSequence = AppendFolder(itemAuthor, seqname);
				m_books->SetItemBold(itemSequence, true);
			} else itemParent = itemAuthor;
		}
		AppendBook(itemParent, data);
		books.Add(data.GetId());
	}
}

wxTreeItemId ExternalDlg::AppendFolder(const wxTreeItemId &parent, const wxString & name)
{
	wxString newname = name;
	newname = newname.Trim(false).Trim(true);
	newname = Normalize(newname, FbParams::GetValue(FB_TRANSLIT_FOLDER));
	wxTreeItemId item = m_books->AppendItem(parent, newname );
	m_books->SetItemBold(item, true);
	m_books->Expand(parent);
	return item;
}

void ExternalDlg::FullNoSequences(wxTreeItemId root, const wxString &selections, bool bUseLetter)
{
	wxString sql = wxT("\
		SELECT books.id, books.title, books.file_size, books.file_type, books.file_name, books.id_author, authors.letter, authors.full_name\
		FROM books \
			LEFT JOIN authors ON authors.id=books.id_author \
		WHERE books.id IN (%s) %s \
		ORDER BY authors.letter, authors.full_name, books.id_author, books.title \
	");
	wxString filter;
	if ( m_checkAuthor && m_checkAuthor->GetValue() )
		filter = wxString::Format(wxT("AND (books.id_author=%d)"), m_author);
	sql = wxString::Format(sql, selections.c_str(), filter.c_str());

	wxArrayInt books;
	wxString thisLeter, thisAuthor;
	wxTreeItemId itemLetter, itemAuthor;

	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
	while (result.NextRow()) {
		BookTreeItemData data(result);
		if ( books.Index(data.GetId()) != wxNOT_FOUND ) continue;
		wxString nextAuthor = result.GetString(wxT("full_name"));
		if (bUseLetter) {
			wxString nextLetter = result.GetString(wxT("letter"));
			if (thisLeter!= nextLetter || !itemLetter.IsOk()) {
				thisLeter = nextLetter;
				itemAuthor = 0L;
				itemLetter = AppendFolder(root, thisLeter);
				m_books->SetItemBold(itemLetter, true);
			}
		} else {
			itemLetter = root;
		}
		if (thisAuthor != nextAuthor || !itemAuthor.IsOk()) {
			thisAuthor = nextAuthor;
			itemAuthor = AppendFolder(itemLetter, thisAuthor);
			m_books->SetItemBold(itemAuthor, true);
		}
		AppendBook(itemAuthor, data);
		books.Add(data.GetId());
	}
}

void ExternalDlg::AppendBook(const wxTreeItemId &parent, BookTreeItemData &data)
{
	wxTreeItemId item = m_books->AppendItem(parent, GetFilename(parent, data), -1, -1, new BookTreeItemData(data));
	m_books->SetItemText (item, 1, wxString::Format(wxT("%d "), data.file_size*m_scale/100/1024));
	m_books->Expand(parent);
}

void ExternalDlg::OnSelectDir( wxCommandEvent& event )
{
	wxDirDialog dlg(
		this,
		_("Select a destination folder"),
		m_textDir->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK) m_textDir->SetValue(dlg.GetPath());
}

void ExternalDlg::OnBookCollapsing(wxTreeEvent & event)
{
	event.Veto();
}

void ExternalDlg::ChangeFilesExt(const wxTreeItemId &parent)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_books->GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*) m_books->GetItemData(child);
		if (data && data->GetId()) {
			m_books->SetItemText(child, 0, GetFilename(parent, *data));
			m_books->SetItemText(child, 1, wxString::Format(wxT("%d"), data->file_size*m_scale/100/1024));
		}
		ChangeFilesExt(child);
		child = m_books->GetNextChild(parent, cookie);
	}
}

void ExternalDlg::OnChangeFormat( wxCommandEvent& event )
{
	int format = m_format->GetCurrentData();
	m_scale = format < 0 ? 43 : 100; // при сжатии средний коэффициент 0.43
	switch (format) {
		case -1: m_ext = wxT(".zip"); break;
		case -2: m_ext = wxT(".gz"); break;
		default: m_ext.Empty();
	}
	m_filenames.Empty();
	ChangeFilesExt(m_books->GetRootItem());
}

void ExternalDlg::FillFilelist(const wxTreeItemId &parent, ExportFileArray &filelist, const wxString &dir)
{
	wxString new_path;
	if (dir.IsEmpty()) {
		new_path = m_books->GetItemText(parent);
	} else {
		wxFileName new_dir = m_books->GetItemText(parent);
		new_dir.SetPath(dir);
		new_path = new_dir.GetFullPath();
	}
	if (!wxFileName::DirExists(new_path)) wxFileName::Mkdir(new_path);

	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_books->GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		wxFileName filename = m_books->GetItemText(child);
		filename.SetPath(new_path);
		BookTreeItemData * data = (BookTreeItemData*) m_books->GetItemData(child);
		if (data && data->GetId()) {
			filelist.Add(new ExportFileItem(filename, data->GetId()));
		} else {
			FillFilelist(child, filelist, new_path);
		}
		child = m_books->GetNextChild(parent, cookie);
	}
}

bool ExternalDlg::ExportBooks()
{
	wxString root_dir = m_textDir->GetValue();
	if (!wxFileName::DirExists(root_dir)) {
		FbMessageBox(_("Destination folder not found"), root_dir);
		return false;
	}
	if (!wxFileName::IsDirWritable(root_dir)) {
		FbMessageBox(_("Unable write files to destination folder"), root_dir);
		return false;
	}

	m_books->SetItemText(m_books->GetRootItem(), root_dir);

	FbExportDlg * dlg = new FbExportDlg(wxGetApp().GetTopWindow(), wxID_ANY, wxT("Export files"));
	FillFilelist(m_books->GetRootItem(), dlg->m_filelist);
	dlg->SetSize(GetSize());
	dlg->SetPosition(GetPosition());
	dlg->m_format = m_format->GetCurrentData();
	dlg->Execute();

	return true;
}

bool ExternalDlg::Execute(wxWindow* parent, FbBookPanel * books, int iAuthor)
{
	wxString selections = books->GetSelected();

	if ( selections.IsEmpty() ) {
		wxMessageBox(_("Not selected any book"));
		return false;
	}

	ExternalDlg dlg(parent, selections, iAuthor);
	dlg.FillBooks(selections);
	return (dlg.ShowModal() == wxID_OK) && dlg.ExportBooks();
}

void ExternalDlg::OnCheckAuthor( wxCommandEvent& event )
{
	FillBooks(m_selections);
}


