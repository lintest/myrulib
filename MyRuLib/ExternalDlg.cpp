///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/arrimpl.cpp>
#include "ExternalDlg.h"
#include "FbParams.h"

///////////////////////////////////////////////////////////////////////////

WX_DEFINE_OBJARRAY(TreeItemArray);

BEGIN_EVENT_TABLE( ExternalDlg, wxDialog )
	EVT_BUTTON( ID_DIR_BTN, ExternalDlg::OnSelectDir )
    EVT_TREE_ITEM_COLLAPSING( ID_BOOKS, ExternalDlg::OnBookCollapsing )
	EVT_CHOICE( wxID_ANY, ExternalDlg::OnChangeFormat )
END_EVENT_TABLE()

extern wxString strOtherSequence;

const wxString strNormalSymbols = wxT("\
.()-_0123456789 \
ABCDEFGHIJKLMNOPQRSTUVWXWZ\
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

wxString ExternalDlg::GetFilename(BookTreeItemData &data)
{
    const wxString filename = data.title;

    size_t size = sizeof(strTranslitArray) / sizeof(LetterReplace);

    wxString newname;
    for (size_t i=0; i<filename.Len(); i++) {
        wxChar letter = filename[i];
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

    while (newname.Left(1) == wxT(".")) newname = newname.Mid(1);
    while (newname.Right(1) == wxT(".")) newname = newname.Mid(0, newname.Len()-1);

    if (data.number) newname = wxString::Format(wxT("%d_%s"), data.number, newname.c_str());

    wxFileName result = wxString::Format(wxT("%s.%s%s"), newname.c_str(), data.file_type.c_str(), m_ext.c_str());

    for (int i=1; true; i++) {
        result.Normalize(wxPATH_NORM_DOTS);
        wxFileName searchname = result;
        searchname.Normalize(wxPATH_NORM_CASE);
        if (m_filenames.Index(searchname.GetFullName()) == wxNOT_FOUND) {
            m_filenames.Add(searchname.GetFullName());
            break;
        }
        result = wxString::Format(wxT("%s(%d).%s%s"), newname.c_str(), i, data.file_type.c_str(), m_ext.c_str());
    }

    return result.GetFullName();
}

wxString ExternalDlg::NormalizeDirname(const wxString &filename)
{
    wxString newname;
    for (size_t i=0; i<filename.Len(); i++) {
        wxChar letter = filename[i];
        if (strNormalSymbols.Find(letter) != wxNOT_FOUND) newname += letter;
    }

    while (newname.Left(1) == wxT(".")) newname = newname.Mid(1);
    while (newname.Right(1) == wxT(".")) newname = newname.Mid(0, newname.Len()-1);

    return newname;
}

ExternalDlg::ExternalDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    SetTitle(_("Экспорт на внешнее устройство"));

	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerDir;
	bSizerDir = new wxBoxSizer( wxHORIZONTAL );

	m_staticTextDir = new wxStaticText( this, wxID_ANY, _("Папка внешнего устройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDir->Wrap( -1 );
	bSizerDir->Add( m_staticTextDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_textDir = new wxTextCtrl( this, ID_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textDir->SetMinSize( wxSize( 300,-1 ) );

	bSizerDir->Add( m_textDir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButtonDir = new wxBitmapButton( this, ID_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerDir->Add( m_bpButtonDir, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerDir, 0, wxEXPAND, 5 );

	long treeStyle = wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_books = new BookListCtrl( this, ID_BOOKS, treeStyle );
	m_books->SetMinSize( wxSize( -1,250 ) );
    m_books->AddColumn (_T("Имя файла"), 400, wxALIGN_LEFT);
    m_books->AddColumn (_T("Размер, Кб"), 100, wxALIGN_RIGHT);
    m_books->colSizes.Add(4);
    m_books->colSizes.Add(1);

	bSizerMain->Add( m_books, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFormat;
	bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	m_staticTextFormat = new wxStaticText( this, wxID_ANY, _("Формат выгрузки файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFormat->Wrap( -1 );
	bSizerFormat->Add( m_staticTextFormat, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_choiceFormatChoices[] = { _("filename.fb2"), _("filename.fb2.zip") };
	int m_choiceFormatNChoices = sizeof( m_choiceFormatChoices ) / sizeof( wxString );
	m_choiceFormat = new wxChoice( this, ID_FORMAT, wxDefaultPosition, wxDefaultSize, m_choiceFormatNChoices, m_choiceFormatChoices, 0 );
	m_choiceFormat->SetSelection( 0 );
	bSizerFormat->Add( m_choiceFormat, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerFormat, 0, wxEXPAND, 5 );

	m_sdbSizerBtn = new wxStdDialogButtonSizer();
	m_sdbSizerBtnOK = new wxButton( this, wxID_OK );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnOK );
	m_sdbSizerBtnCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnCancel );
	m_sdbSizerBtn->Realize();
	bSizerMain->Add( m_sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	m_textDir->SetValue( FbParams().GetText(FB_EXTERNAL_DIR) );
	m_choiceFormat->SetSelection( FbParams().GetValue(FB_FILE_FORMAT) );
}

ExternalDlg::~ExternalDlg()
{
}

void ExternalDlg::ScanChilds(wxTreeListCtrl* bookList, const wxTreeItemId &root, TreeItemArray &selections)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = bookList->GetFirstChild(root, cookie);
    while (child.IsOk()) {
        if (bookList->GetItemImage(child) == 1) {
            BookTreeItemData * data = (BookTreeItemData*) bookList->GetItemData(child);
            if (data && data->GetId()) selections.Add(new BookTreeItemData(data));
        }
        ScanChilds(bookList, child, selections);
        child = bookList->GetNextChild(root, cookie);
    }

    if (selections.Count()) return;

    wxArrayTreeItemIds itemArray;
    size_t count = bookList->GetSelections(itemArray);
    for (size_t i=0; i<count; ++i) {
        BookTreeItemData * data = (BookTreeItemData*) bookList->GetItemData(itemArray[i]);
        if (data && data->GetId()) selections.Add(new BookTreeItemData(data));
    }
}

void ExternalDlg::FillBooks(const wxString &author, TreeItemArray &selections)
{
    wxString ext;
    if (m_choiceFormat->GetCurrentSelection() == 0) {
        m_ext = wxEmptyString;
        m_scale = 100;
    } else {
        m_ext = wxT(".zip");
        m_scale = 43;
    }
    m_filenames.Empty();

    wxTreeItemId root = m_books->AddRoot(NormalizeDirname(author));
    m_books->SetItemBold(root, true);

    if (FbParams().GetValue(FB_FOLDER_FORMAT) == 0) {
        wxSortedArrayString sequences;
        bool otherExisis = false;
        for  (size_t i=0; i<selections.Count(); i++) {
            wxString sequence = selections[i].sequence;
            if (sequence.IsEmpty()) {
                otherExisis = true;
            } else if (sequences.Index(sequence, false) == wxNOT_FOUND) {
                sequences.Add(selections[i].sequence);
            }
        }
        sequences.Sort();

        for  (size_t i=0; i<sequences.Count(); i++) {
            wxTreeItemId folder = m_books->AppendItem(root, NormalizeDirname(sequences[i]));
            m_books->SetItemBold(folder, true);
            for  (size_t j=0; j<selections.Count(); j++) {
                if (sequences[i] == selections[j].sequence) AppendBook(folder, selections[j]);
            }
        }

        if (otherExisis) {
            wxTreeItemId folder = m_books->AppendItem(root, NormalizeDirname(strOtherSequence));
            m_books->SetItemBold(folder, true);
            for  (size_t j=0; j<selections.Count(); j++) {
                if (selections[j].sequence.IsEmpty()) AppendBook(folder, selections[j]);
            }
        }
    } else {
        for  (size_t j=0; j<selections.Count(); j++) {
            AppendBook(root, selections[j]);
        }
    }

    m_books->ExpandAll(root);
}

void ExternalDlg::AppendBook(const wxTreeItemId &parent, BookTreeItemData &data)
{
    wxTreeItemId item = m_books->AppendItem(parent, GetFilename(data), -1, -1, new BookTreeItemData(data));
    m_books->SetItemText (item, 1, wxString::Format(wxT("%d"), data.file_size*m_scale/100/1024));
}

bool ExternalDlg::Execute(wxWindow* parent, wxTreeListCtrl* bookList, const wxString &author)
{
    TreeItemArray selections;
    wxTreeItemId root = bookList->GetRootItem();
    ScanChilds(bookList, root, selections);

    if (!selections.Count()) {
        wxMessageBox(wxT("Не выбрано ни одной книги."));
        return false;
    }

    ExternalDlg dlg(parent);
    dlg.FillBooks(author, selections);

    if (dlg.ShowModal() == wxID_OK)
        return dlg.ExportBooks();
    else
        return false;
}

void ExternalDlg::OnSelectDir( wxCommandEvent& event )
{
    wxDirDialog dlg(
        this,
        _("Выберите папку внешнего устройства"),
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
            m_books->SetItemText(child, 0, GetFilename(*data));
            m_books->SetItemText(child, 1, wxString::Format(wxT("%d"), data->file_size*m_scale/100/1024));
        }
        ChangeFilesExt(child);
        child = m_books->GetNextChild(parent, cookie);
    }
}

void ExternalDlg::OnChangeFormat( wxCommandEvent& event )
{
    wxString ext;
    if (m_choiceFormat->GetCurrentSelection() == 0) {
        m_ext = wxEmptyString;
        m_scale = 100;
    } else {
        m_ext = wxT(".zip");
        m_scale = 43;
    }
    m_filenames.Empty();

    // при сжатии средний коэффициент 0.43
    ChangeFilesExt(m_books->GetRootItem());
}

void ExternalDlg::FillFilelist(const wxTreeItemId &parent, ExportFileArray &filelist, const wxString &dir)
{
    wxFileName new_dir = m_books->GetItemText(parent);
    new_dir.SetPath(dir);
    wxString new_path = new_dir.GetFullPath();
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
        wxMessageBox(_("Не найдена папка внешнего устройства: ") + root_dir);
        return false;
    }
    if (!wxFileName::IsDirWritable(root_dir)) {
        wxMessageBox(_("Папка внешнего устройства не доступна для записи: ") + root_dir);
        return false;
    }

	ExportThread *thread = new ExportThread(GetParent(), m_choiceFormat->GetCurrentSelection());
	FillFilelist(m_books->GetRootItem(), thread->m_filelist, m_textDir->GetValue());

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

    return true;
}