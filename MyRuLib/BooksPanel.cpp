#include "BooksPanel.h"
#include "ExternalDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "InfoCash.h"
#include "MyRuLibApp.h"
#include "db/Sequences.h"
#include "db/Bookseq.h"

BEGIN_EVENT_TABLE(BooksPanel, wxSplitterWindow)
	EVT_MENU(ID_SPLIT_HORIZONTAL, BooksPanel::OnChangeView)
	EVT_MENU(ID_SPLIT_VERTICAL, BooksPanel::OnChangeView)
	EVT_MENU(wxID_SELECTALL, BooksPanel::OnSelectAll)
    EVT_MENU(wxID_SAVE, BooksPanel::OnExternal)
    EVT_MENU(ID_BOOKINFO_UPDATE, BooksPanel::OnInfoUpdate)
    EVT_TREE_SEL_CHANGED(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListViewSelected)
	EVT_TREE_ITEM_ACTIVATED(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListActivated)
	EVT_TREE_ITEM_COLLAPSING(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListCollapsing)
	EVT_TREE_KEY_DOWN(ID_BOOKS_LISTCTRL, BooksPanel::OnBooksListKeyDown)
	EVT_TREE_STATE_IMAGE_CLICK(ID_BOOKS_LISTCTRL, BooksPanel::OnImageClick)
END_EVENT_TABLE()

BooksPanel::BooksPanel(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    :wxSplitterWindow(parent, id, pos, size, style, name), m_BookInfo(NULL)
{
	SetMinimumPaneSize(50);
	SetSashGravity(0.5);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	m_BookList = new BookListCtrl(this, ID_BOOKS_LISTCTRL, substyle);

	CreateBookInfo();

    #if defined(__WIN32__)
	m_BookInfo->SetPage(_("<b>Внимание!</b><br><br>Версия для Windows не поддерживает<br>архивы размером более 2 Gb."));
    #endif
}

void BooksPanel::CreateBookInfo()
{
    int vertical = FbParams().GetValue(FB_VIEW_TYPE);

	if (m_BookInfo) Unsplit(m_BookInfo);

	m_BookInfo = new wxHtmlWindow(this, ID_BOOKS_INFO_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	int fontsizes[] = {6, 8, 9, 10, 12, 16, 18};
	m_BookInfo->SetFonts(wxT("Tahoma"), wxT("Tahoma"), fontsizes);

	if (vertical)
		SplitVertically(m_BookList, m_BookInfo, GetSize().GetWidth()/2);
	else
		SplitHorizontally(m_BookList, m_BookInfo, GetSize().GetHeight()/2);

    BookTreeItemData * book = GetSelectedBook();
    if (!book) {
        m_BookInfo->SetPage(wxEmptyString);
    } else {
        wxString html = InfoCash::GetInfo(book->GetId(), vertical);
        m_BookInfo->SetPage(html);
        InfoCash::ShowInfo(this, book->GetId(), book->file_type);
    }
}

BookTreeItemData * BooksPanel::GetSelectedBook()
{
	wxTreeItemId selected = m_BookList->GetSelection();
	if (selected.IsOk()) {
		return (BookTreeItemData*) m_BookList->GetItemData(selected);
    } else
        return NULL;
}

void BooksPanel::OnBooksListViewSelected(wxTreeEvent & event)
{
    m_BookInfo->SetPage(wxEmptyString);

	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*) m_BookList->GetItemData(selected);
		if (data) InfoCash::ShowInfo(this, data->GetId(), data->file_type);
	}
}

void BooksPanel::OnBooksListCollapsing(wxTreeEvent & event)
{
	event.Veto();
}

void BooksPanel::OnImageClick(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		int image = m_BookList->GetItemImage(item);
		image = ( image == 1 ? 0 : 1);
		if (m_BookList->GetItemBold(item)) {
			m_BookList->SetItemImage(item, image);
			wxTreeItemIdValue cookie;
			item = m_BookList->GetFirstChild(item, cookie);
			while (item.IsOk()) {
				wxTreeItemIdValue cookie;
				wxTreeItemId subitem = m_BookList->GetFirstChild(item, cookie);
				while (subitem.IsOk()) {
					m_BookList->SetItemImage(subitem, image);
					subitem = m_BookList->GetNextSibling(subitem);
				}
				m_BookList->SetItemImage(item, image);
				item = m_BookList->GetNextSibling (item);
			}
		} else {
			m_BookList->SetItemImage(item, image);
			wxTreeItemId parent = m_BookList->GetItemParent(item);
			wxTreeItemIdValue cookie;
			item = m_BookList->GetFirstChild(parent, cookie);
			while (item.IsOk()) {
				if (image != m_BookList->GetItemImage(item)) {
					image = 2;
					break;
				}
				item = m_BookList->GetNextSibling (item);
			}
			m_BookList->SetItemImage(parent, image);
		}
	}
	event.Veto();
}

void BooksPanel::OnChangeView(wxCommandEvent & event)
{
	int vertical = (event.GetId() == ID_SPLIT_VERTICAL);
	FbParams().SetValue(FB_VIEW_TYPE, vertical);
	CreateBookInfo();
}

void BooksPanel::OnSelectAll(wxCommandEvent& event)
{
    m_BookList->SelectAll();
}

void BooksPanel::OnBooksListActivated(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*)m_BookList->GetItemData(selected);
		if (data) FbManager::OpenBook(data->GetId(), data->file_type);
	}
}

void BooksPanel::OnBooksListKeyDown(wxTreeEvent & event)
{
	if (event.GetKeyCode() == 0x20) {
		wxArrayTreeItemIds selections;
		size_t count = m_BookList->GetSelections(selections);
		int image = 0;
		for (size_t i=0; i<count; ++i) {
            wxTreeItemId selected = selections[i];
		    if (i==0)
                image = (m_BookList->GetItemImage(selected) + 1) % 2;
			m_BookList->SetItemImage(selected, image);
		}
		event.Veto();
	} else
		event.Skip();
}

void BooksPanel::OnInfoUpdate(wxCommandEvent& event)
{
    int vertical = FbParams().GetValue(FB_VIEW_TYPE);

	wxTreeItemId selected = m_BookList->GetSelection();
	if (selected.IsOk()) {
		BookTreeItemData * data= (BookTreeItemData*)m_BookList->GetItemData(selected);
		if (data && (data->GetId() == event.GetInt())) {
            wxString html = InfoCash::GetInfo(event.GetInt(), vertical);
            m_BookInfo->SetPage(html);
		}
	}
}

void BooksPanel::OnExternal(wxCommandEvent& event)
{
    ExternalDlg::Execute(this, m_BookList, m_AuthorName);
}

void BooksPanel::FillByAuthor(int id_author)
{
    bool fb2only = false;

	m_BookList->Freeze();

    m_BookList->DeleteRoot();
    wxTreeItemId root = m_BookList->AddRoot (_T("Root"));

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * thisAuthor = authors.Id(id_author);
	if(thisAuthor)
	{
	    m_AuthorName = thisAuthor->full_name;
		root = m_BookList->AppendItem(root, thisAuthor->full_name, 0);
		m_BookList->SetItemBold(root, true);

		BookseqRowSet * bookseq = thisAuthor->GetBookseqs();

		int id_seq = 0;
		wxString sequencesText;
		for(size_t i = 0; i < bookseq->Count(); i++){
		    BookseqRow * seqRow = bookseq->Item(i);
			if (id_seq == seqRow->id_seq) continue;

            if (!sequencesText.IsEmpty()) sequencesText += wxT(",");
            id_seq = seqRow->id_seq;
            sequencesText += wxString::Format(wxT("%d"), id_seq);
		}

		BooksRowSet * allBooks = thisAuthor->GetBooks(wxT("title"));
		for(size_t i = 0; i < allBooks->Count(); i++) {
            allBooks->Item(i)->added = false;
		}

		if (!sequencesText.IsEmpty()) {
			wxString whereCause = wxString::Format(wxT("id in(%s)"), sequencesText.c_str());
			Sequences sequences(wxGetApp().GetDatabase());
			SequencesRowSet * allSequences = sequences.WhereSet(whereCause, wxT("value"));

			for(size_t i = 0; i < allSequences->Count(); i++) {
				SequencesRow * thisSequence = allSequences->Item(i);
				wxTreeItemId parent = m_BookList->AppendItem(root, thisSequence->value, 0);
				m_BookList->SetItemBold(parent, true);

                for (size_t j = 0; j < bookseq->Count(); j++) {
                    BookseqRow * seqRow = bookseq->Item(j);
                    if (seqRow->id_seq != thisSequence->id) continue;

                    for(size_t k = 0; k < allBooks->Count(); k++) {
                        BooksRow * thisBook = allBooks->Item(k);
                        if (seqRow->id_book == thisBook->id) seqRow->order = k;
                    }
                }
                bookseq->SortBy(wxT("number,order"));

                for (size_t j = 0; j < bookseq->Count(); j++) {
                    BookseqRow * seqRow = bookseq->Item(j);
                    if (seqRow->id_seq != thisSequence->id) continue;

                    for(size_t k = 0; k < allBooks->Count(); k++) {
                        BooksRow * thisBook = allBooks->Item(k);
                        if (seqRow->id_book != thisBook->id) continue;
                        if (fb2only && thisBook->file_type != wxT("fb2")) continue;

                        wxTreeItemId item = m_BookList->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook, thisSequence->value, seqRow->number));
                        if (seqRow->number>0) m_BookList->SetItemText (item, 1, wxString::Format(wxT("%d"), seqRow->number));
                        m_BookList->SetItemText (item, 2, thisBook->file_name);
                        m_BookList->SetItemText (item, 3, wxString::Format(wxT("%d"), thisBook->file_size/1024));
                        thisBook->added = true;
                    }
                }
			}
		}

		wxTreeItemId parent = root;
		for(size_t i = 0; i < allBooks->Count(); i++) {
		    BooksRow * thisBook = allBooks->Item(i);
		    if (thisBook->added) continue;
            if (fb2only && thisBook->file_type != wxT("fb2")) continue;
		    if (parent == root) {
                parent = m_BookList->AppendItem(root, strOtherSequence, 0);
                m_BookList->SetItemBold(parent, true);
		    }
            wxTreeItemId item = m_BookList->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook));
            m_BookList->SetItemText (item, 2, thisBook->file_name);
            m_BookList->SetItemText (item, 3, wxString::Format(wxT("%d"), thisBook->file_size/1024));
		}
	}
    m_BookList->ExpandAll(root);

	m_BookList->Thaw();

	m_BookInfo->SetPage(wxEmptyString);
}

void BooksPanel::FillByFind(const wxString &title, const wxString &author)
{
	m_BookList->Freeze();

    m_BookList->DeleteRoot();
    wxString msg = wxString::Format(_T("Поиск: %s %s"), title.c_str(), author.c_str());
    wxTreeItemId root = m_BookList->AddRoot(msg);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    DatabaseLayer * database = wxGetApp().GetDatabase();
/*
    {
        wxString sql = wxT("SELECT COUNT(id) FROM books");
        PreparedStatement* ps = database->PrepareStatement(sql);
        DatabaseResultSet* result = ps->ExecuteQuery();
        int count = 0;
        if (result && result->Next()) count = result->GetResultInt(1);
        database->CloseResultSet(result);
        database->CloseStatement(ps);

        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_START );
        event.SetString(msg);
        event.SetInt(count);
        wxPostEvent(wxGetApp().GetTopWindow(), event);
    }
*/
	wxString sql = wxT("SELECT id, title, file_name, file_type, file_size FROM books WHERE LOWER(title) like ? LIMIT 1024");
	PreparedStatement* ps = database->PrepareStatement(sql);
	ps->SetParamString(1, wxT("%") + title + wxT("%"));
	DatabaseResultSet* result = ps->ExecuteQuery();

    while (result && result->Next()) {
        BookTreeItemData * data = new BookTreeItemData(result);
        wxTreeItemId item = m_BookList->AppendItem(root, data->title, 0, -1, data);
        m_BookList->SetItemText (item, 2, data->file_name);
        m_BookList->SetItemText (item, 3, wxString::Format(wxT("%d"), data->file_size/1024));
    }

	database->CloseResultSet(result);
	database->CloseStatement(ps);

    m_BookList->ExpandAll(root);
	m_BookList->Thaw();

	m_BookInfo->SetPage(wxEmptyString);
}

