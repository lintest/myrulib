///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

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

extern wxString strOtherSequence;

void ExternalDlg::FillBooks(const wxString &author, TreeItemArray &selections)
{
    wxTreeItemId root = m_books->AddRoot(author);
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
            wxTreeItemId folder = m_books->AppendItem(root, sequences[i]);
            m_books->SetItemBold(folder, true);
            for  (size_t j=0; j<selections.Count(); j++) {
                if (sequences[i] == selections[j].sequence) AppendBook(folder, selections[j]);
            }
        }

        if (otherExisis) {
            wxTreeItemId folder = m_books->AppendItem(root, strOtherSequence);
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
    wxTreeItemId item = m_books->AppendItem(parent, data.title, -1, -1, new BookTreeItemData(data));
    m_books->SetItemText (item, 1, wxString::Format(wxT("%d"), data.file_size/1024));
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
    bool result = (dlg.ShowModal() == wxID_OK);

    if (result) dlg.ExportBooks();

    return result;
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

void ExternalDlg::ExportBooks()
{
}

void ExternalDlg::OnChangeFormat( wxCommandEvent& event )
{
}
