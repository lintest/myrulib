#include "FbCheckList.h"
#include "FbLogoBitmap.h"
#include <wx/imaglist.h>

BEGIN_EVENT_TABLE(FbCheckList, FbTreeListCtrl)
	EVT_TREE_KEY_DOWN(wxID_ANY, FbCheckList::OnKeyDown)
	EVT_TREE_STATE_IMAGE_CLICK(wxID_ANY, FbCheckList::OnImageClick)
	EVT_TREE_ITEM_COLLAPSING(wxID_ANY, FbCheckList::OnCollapsing)
END_EVENT_TABLE()

FbCheckList::FbCheckList(wxWindow *parent, wxWindowID id, long style)
	:FbTreeListCtrl(parent, id, style)
{
	wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	images->Add (wxBitmap(checkout_xpm));
	AssignImageList (images);
};

void FbCheckList::SelectAll(int image)
{
	wxTreeItemId root = GetRootItem();
	if (root.IsOk()) SelectChild(root, image);
}

void FbCheckList::SelectChild(const wxTreeItemId &parent, int image)
{
	SetItemImage(parent, image);
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		SelectChild(child, image);
		child = GetNextChild(parent, cookie);
	}
}

void FbCheckList::SelectParent(const wxTreeItemId &item)
{
	wxTreeItemId parent = GetItemParent(item);
	if (!parent.IsOk()) return;

	int all = -1;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int image = GetItemImage(child);
		if (all < 0) all = image; else if (all != image) all = 2;
		child = GetNextChild(parent, cookie);
	}
	SetItemImage(parent, all);
	SelectParent(parent);
}

void FbCheckList::OnImageClick(wxTreeEvent &event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		int image = GetItemImage(selected);
		image = ( image == 1 ? 0 : 1);
		SelectChild(selected, image);
		SelectParent(selected);
	}
	event.Veto();
}

void FbCheckList::OnKeyDown(wxTreeEvent & event)
{
	if (event.GetKeyCode() == 0x20) {
		wxArrayTreeItemIds selections;
		size_t count = GetSelections(selections);
		int image = 0;
		for (size_t i=0; i<count; ++i) {
			wxTreeItemId selected = selections[i];
			if (i==0) image = (GetItemImage(selected) + 1) % 2;
			SelectChild(selected, image);
			SelectParent(selected);
		}
		event.Veto();
	} else event.Skip();
}

void FbCheckList::OnCollapsing(wxTreeEvent & event)
{
	event.Veto();
}

