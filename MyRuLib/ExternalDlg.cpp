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

const wxString strNormalSymbols = wxT("\
.()-_0123456789 \
ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz\
АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\
абвгдеёжзийклмнопрстуфхцчшщъыьэюя\
");

struct LetterReplace {
	wxChar ru;
	wxString en;
};

const LetterReplace strTranslitArray[] = {
	{wxT(' '), wxT("_")},
	{wxT('А'), wxT("A")},
	{wxT('Б'), wxT("B")},
	{wxT('В'), wxT("W")},
	{wxT('Г'), wxT("G")},
	{wxT('Д'), wxT("D")},
	{wxT('Е'), wxT("E")},
	{wxT('Ё'), wxT("E")},
	{wxT('Ж'), wxT("V")},
	{wxT('З'), wxT("Z")},
	{wxT('И'), wxT("I")},
	{wxT('Й'), wxT("J")},
	{wxT('К'), wxT("K")},
	{wxT('Л'), wxT("L")},
	{wxT('М'), wxT("M")},
	{wxT('Н'), wxT("N")},
	{wxT('О'), wxT("O")},
	{wxT('П'), wxT("P")},
	{wxT('Р'), wxT("R")},
	{wxT('С'), wxT("S")},
	{wxT('Т'), wxT("T")},
	{wxT('У'), wxT("U")},
	{wxT('Ф'), wxT("F")},
	{wxT('Х'), wxT("X")},
	{wxT('Ц'), wxT("C")},
	{wxT('Ч'), wxT("CH")},
	{wxT('Ш'), wxT("SH")},
	{wxT('Щ'), wxT("SCH")},
	{wxT('Ъ'), wxT("")},
	{wxT('Ы'), wxT("Y")},
	{wxT('Ь'), wxT("")},
	{wxT('Э'), wxT("E")},
	{wxT('Ю'), wxT("JU")},
	{wxT('Я'), wxT("JA")},
	{wxT('а'), wxT("a")},
	{wxT('б'), wxT("b")},
	{wxT('в'), wxT("w")},
	{wxT('г'), wxT("g")},
	{wxT('д'), wxT("d")},
	{wxT('е'), wxT("e")},
	{wxT('ё'), wxT("e")},
	{wxT('ж'), wxT("v")},
	{wxT('з'), wxT("z")},
	{wxT('и'), wxT("i")},
	{wxT('й'), wxT("j")},
	{wxT('к'), wxT("k")},
	{wxT('л'), wxT("l")},
	{wxT('м'), wxT("m")},
	{wxT('н'), wxT("n")},
	{wxT('о'), wxT("o")},
	{wxT('п'), wxT("p")},
	{wxT('р'), wxT("r")},
	{wxT('с'), wxT("s")},
	{wxT('т'), wxT("t")},
	{wxT('у'), wxT("u")},
	{wxT('ф'), wxT("f")},
	{wxT('х'), wxT("x")},
	{wxT('ц'), wxT("c")},
	{wxT('ч'), wxT("ch")},
	{wxT('ш'), wxT("sh")},
	{wxT('щ'), wxT("sch")},
	{wxT('ъ'), wxT("")},
	{wxT('ы'), wxT("y")},
	{wxT('ь'), wxT("")},
	{wxT('э'), wxT("e")},
	{wxT('ю'), wxT("ju")},
	{wxT('я'), wxT("ja")}
};


wxString ExternalDlg::Translit(const wxString &filename)
{
	wxString oldname = filename;
	oldname = oldname.Trim(false).Trim(true);

	while (oldname.Left(1) == wxT(".")) oldname = oldname.Mid(1);
	while (oldname.Right(1) == wxT(".")) oldname = oldname.Mid(0, oldname.Len()-1);

	wxString newname;
	size_t size = sizeof(strTranslitArray) / sizeof(LetterReplace);
	for (size_t i=0; i<oldname.Len(); i++) {
		wxChar letter = oldname[i];
		if (strNormalSymbols.Find(letter) != wxNOT_FOUND) {
			wxString substr = letter;
			for (size_t j=0; j<size; j++)
				if (strTranslitArray[j].ru == letter) {
					substr = strTranslitArray[j].en;
					break;
				}
			newname += substr;
		}
	}
	return newname;
}

wxString ExternalDlg::Normalize(const wxString &filename)
{
	wxString newname = filename;
	newname = newname.Trim(false).Trim(true).Left(fbMAX_FILENAME_LENGTH);

	const wxChar cp1251 [256] =
	{
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x005F, 0x0021, 0x0027, 0x0000, 0x0000, 0x0000, 0x0000, 0x0027, 0x0028, 0x0029, 0x002D, 0x002D, 0x0000, 0x002D, 0x002E, 0x0000,
		0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x0000, 0x0000, 0x0000, 0x002D, 0x0000, 0x0000,
		0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
		0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0028, 0x0000, 0x0029, 0x005E, 0x005F,
		0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
		0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0028, 0x0000, 0x0029, 0x007E, 0x0000,

		0x0044, 0x0413, 0x0027, 0x0453, 0x0027, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x041B, 0x0027, 0x041D, 0x041A, 0x0048, 0x0426,
		0x0064, 0x0027, 0x0027, 0x0027, 0x0027, 0x002D, 0x002D, 0x0000, 0x0000, 0x0000, 0x043B, 0x0027, 0x043D, 0x043A, 0x0068, 0x0446,
		0x005F, 0x0423, 0x0443, 0x004A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0415, 0x0000, 0x0415, 0x0027, 0x0000, 0x0000, 0x0000, 0x0049,
		0x0000, 0x0000, 0x0049, 0x0069, 0x0433, 0x0000, 0x0000, 0x0000, 0x0435, 0x004E, 0x0435, 0x0027, 0x006A, 0x0053, 0x0073, 0x0069,
		0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
		0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
		0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
		0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
	}; // cp1251

	char * buffer[fbMAX_FILENAME_LENGTH];
	memset(buffer, 0, (fbMAX_FILENAME_LENGTH));

	wxEncodingConverter ec;
	ec.Init(wxFONTENCODING_UNICODE, wxFONTENCODING_CP1251, wxCONVERT_SUBSTITUTE);
	newname = ec.Convert(newname);

	wxString result;
	bool space = false;
	size_t count = newname.Len();
	for (size_t i = 0; i < count; i++) {
		unsigned char ch = (unsigned char) newname[i];
		wxChar wch = cp1251[ch];
		if (wch == 0) continue;
		if (space && wch == 0x005F) continue;
		result << wch;
		space = wch == 0x005F;
	}

	while (result.Left(1) == wxT(".")) result = result.Mid(1);
	while (result.Right(1) == wxT(".")) result = result.Mid(0, result.Len()-1);

	return result;
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

	newname = FbParams::GetValue(FB_TRANSLIT_FILE) ? Translit(newname) : Normalize(newname);

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
	newname = FbParams::GetValue(FB_TRANSLIT_FOLDER) ? Translit(newname) : Normalize(newname);
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


