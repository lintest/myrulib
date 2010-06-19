#include "FbBookList.h"
#include "FbBookData.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbBookListThread
//-----------------------------------------------------------------------------

void * FbBookListThread::Entry()
{
	try {
		FbCommonDatabase database;
		DoAuthor(database);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbBookListThread::DoAuthor(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id FROM books WHERE id_author=? ORDER BY title");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_author);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbBookListThread::MakeModel(wxSQLite3ResultSet &result)
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		items.Add(result.GetInt(0));
		count++;
		if (count == length) {
			length = fbLIST_ARRAY_SIZE;
			FbArrayEvent(id, items).Post(m_frame);
			id = ID_MODEL_APPEND;
			items.Empty();
			count = 0;
		}
	}
	FbArrayEvent(id, items).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbBookListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookListData, FbModelData)

void FbBookListData::DoSetState(FbModel & model, int state)
{
	FbBookListModel * booklist = wxDynamicCast(&model, FbBookListModel);
	if (booklist) booklist->SetState(m_code, state);
}

int FbBookListData::DoGetState(FbModel & model) const
{
	FbBookListModel * booklist = wxDynamicCast(&model, FbBookListModel);
	return booklist ? booklist->GetState(m_code) : 0;
}

wxString FbBookListData::GetValue(FbModel & model, size_t col) const
{
	return FbCollection::GetBook(m_code, col);
}

//-----------------------------------------------------------------------------
//  FbBookListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookListModel, FbListModel)

FbBookListModel::FbBookListModel(const wxArrayInt &items)
	: m_check(CompareInt), m_data(NULL)
{
	m_position = items.Count() == 0 ? 0 : 1;
	Append(items);
}

FbBookListModel::~FbBookListModel(void)
{
	wxDELETE(m_data);
}

void FbBookListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

void FbBookListModel::SetState(int code, int state)
{
	int index = m_check.Index(code);
	if (index == wxNOT_FOUND) {
		if (state) m_ctrls.Add(m_position);
	} else {
		if (!state) m_ctrls.RemoveAt(index);
	}
}

int FbBookListModel::GetState(int code) const
{
	return m_check.Index(code) != wxNOT_FOUND;
}

FbModelData * FbBookListModel::DoGetData(size_t row, int &level)
{
	level = 0;
	if (row == 0 || row > m_items.Count()) return NULL;
	int code = m_items[row - 1];
	wxDELETE(m_data);
	return m_data = new FbBookListData(code);
}

//-----------------------------------------------------------------------------
//  FbBookList
//-----------------------------------------------------------------------------

FbBookList::FbBookList(wxWindow *parent, wxWindowID id, long style)
	:FbCheckList(parent, id, style)
{
}

BookListUpdater::BookListUpdater(wxTreeListCtrl * list) :m_list(list)
{
	m_list->Freeze();
	m_list->DeleteRoot();
}

BookListUpdater::~BookListUpdater()
{
	m_list->Thaw();
	m_list->Update();
}

int FbBookList::GetItemBook(const wxTreeItemId &item) const
{
	FbItemData * data = (FbItemData*) GetItemData(item);
	return data ? data->GetId() : 0;
}

size_t FbBookList::GetChecked(const wxTreeItemId &parent, wxString &selections)
{
	size_t result = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int id;
		if ( GetItemImage(child) == 1 && (id = GetItemBook(child)) ) {
			if ( !selections.IsEmpty() ) selections += wxT(",");
			selections += wxString::Format(wxT("%d"), id);
			result++;
		}
		result += GetChecked(child, selections);
		child = GetNextChild(parent, cookie);
	}
	return result;
}

size_t FbBookList::GetSelected(wxString &selections)
{
	selections.Empty();
	{
		size_t count = GetChecked(GetRootItem(), selections);
		if (count) return count;
	}
	size_t result = 0;
	wxArrayTreeItemIds array;
	size_t count = FbTreeListCtrl::GetSelections(array);
	for (size_t i=0; i<count; ++i) {
		int id = GetItemBook(array[i]);
		if (id) {
			if ( !selections.IsEmpty() ) selections += wxT(",");
			selections += wxString::Format(wxT("%d"), id);
			result++;
		}
	}
	return result;
}

size_t FbBookList::GetChecked(const wxTreeItemId &parent, wxArrayInt &items)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int id;
		if ( GetItemImage(child) == 1 && (id = GetItemBook(child)) ) items.Add(id);
		GetChecked(child, items);
		child = GetNextChild(parent, cookie);
	}
	return items.Count();
}

size_t FbBookList::GetSelected(wxArrayInt &items)
{
	items.Empty();
	{
		size_t count = GetChecked(GetRootItem(), items);
		if (count) return count;
	}
	wxArrayTreeItemIds array;
	size_t count = FbTreeListCtrl::GetSelections(array);
	for (size_t i=0; i<count; ++i) {
		int id = GetItemBook(array[i]);
		if (id) items.Add(id);
	}
	return items.Count();
}

wxString FbBookList::GetSelected()
{
	wxString selections;
	GetSelected(selections);
	return selections;
}

bool FbBookList::DeleteItems(const wxTreeItemId &parent, wxArrayInt &items)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int id = GetItemBook(child);
		if ( id ) {
			if (items.Index(id) != wxNOT_FOUND) {
				Delete(child);
				return true;
			}
		} else if (!HasChildren(child)) {
			Delete(child);
			return true;
		}
		if (DeleteItems(child, items)) return true;
		child = GetNextChild(parent, cookie);
	}
	return false;
}

void FbBookList::DeleteItems(wxArrayInt &items)
{
	while (DeleteItems(GetRootItem(), items)) {};
}

size_t FbBookList::GetCount(const wxTreeItemId &parent, wxArrayInt &items)
{
	size_t count = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int id = GetItemBook(child);
		if (id && items.Index(id) == wxNOT_FOUND) {
			items.Add(id);
			count++;
		}
		count += GetCount(child, items);
		child = GetNextChild(parent, cookie);
	}
	return count;
}

size_t FbBookList::GetCount()
{
	wxArrayInt items;
	return GetCount(GetRootItem(), items);
}

size_t FbBookList::UpdateRating(wxTreeItemId parent, size_t column, const wxString &rating)
{
	size_t result = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		if (GetItemImage(child) == 1) {
			if ( GetItemBook(child) ) {
				SetItemText(child, column, rating);
				result++;
			}
		}
		result += UpdateRating(child, column, rating);
		child = GetNextChild(parent, cookie);
	}
	return result;
}

size_t FbBookList::UpdateRating(size_t column, const wxString &rating)
{
	{
		size_t count = UpdateRating(GetRootItem(), column, rating);
		if (count) return count;
	}
	wxArrayTreeItemIds items;
	size_t count = GetSelections(items);
	for (size_t i=0; i<count; ++i) {
		if ( GetItemBook(items[i]) ) {
			SetItemText(items[i], column, rating);
		}
	}
	return count;
}

