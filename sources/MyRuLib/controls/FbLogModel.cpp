#include "FbLogModel.h"
#include "FbLogStream.h"
#include "FbTreeView.h"
#include "FbConst.h"
#include <wx/clipbrd.h>

#define MAXIMUM_LOG_ROW_COUNT 1024

//-----------------------------------------------------------------------------
//  FbLogViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbLogViewCtrl, FbTreeViewCtrl)

BEGIN_EVENT_TABLE(FbLogViewCtrl, FbTreeViewCtrl)
	EVT_TREE_ITEM_MENU(wxID_ANY, FbLogViewCtrl::OnContextMenu)
	EVT_MENU(wxID_COPY, FbLogViewCtrl::OnCopy)
	EVT_MENU(wxID_SELECTALL, FbLogViewCtrl::OnSelect)
	EVT_MENU(ID_UNSELECTALL, FbLogViewCtrl::OnUnselect)
	EVT_UPDATE_UI(wxID_CUT, FbLogViewCtrl::OnDisableUI)
	EVT_UPDATE_UI(wxID_COPY, FbLogViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(wxID_PASTE, FbLogViewCtrl::OnDisableUI)
	EVT_UPDATE_UI(wxID_SELECTALL, FbLogViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(ID_UNSELECTALL, FbLogViewCtrl::OnEnableUI)
END_EVENT_TABLE()

void FbLogViewCtrl::OnCopy(wxCommandEvent& event)
{
	FbLogModel * model = wxDynamicCast(GetModel(), FbLogModel);
	if (model == NULL) return;
	wxString text = model->GetSelectedText();
	if (text.IsEmpty()) return;

	wxClipboardLocker locker;
	if (!locker) return;
	wxTheClipboard->SetData( new wxTextDataObject(text) );
}

void FbLogViewCtrl::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ContextMenu menu;
	PopupMenu(&menu, point);
}

FbLogViewCtrl::ContextMenu::ContextMenu()
{
	Append(wxID_COPY, _("Copy") + (wxString)wxT("\tCtrl+C"), wxART_COPY);
	AppendSeparator();
	Append(wxID_SELECTALL, _("Select all") + (wxString)wxT("\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Undo selection"));
	Append(ID_TEXTLOG_HIDE, _("Hide log window") + (wxString)wxT("\tF12"));
}

//-----------------------------------------------------------------------------
//  FbLogData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbLogData, FbModelData)

//-----------------------------------------------------------------------------
//  FbLogModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbLogModel, FbListModel)

FbModelItem FbLogModel::DoGetData(size_t row, int &level)
{
	level = 0;
	if (row == 0 || row > m_lines.Count()) return *this;
	wxString text = m_lines[row - 1];
	FbLogData data(text);
	return FbModelItem(*this, &data);
}

bool FbLogModel::Update()
{
	bool ok = FbLog::Update(m_lines);
	if (ok) {
		size_t count = m_lines.Count();
		if (count > MAXIMUM_LOG_ROW_COUNT) {
			m_lines.RemoveAt(0, count - MAXIMUM_LOG_ROW_COUNT);
			count = m_lines.Count();
		}
		m_position = count;
		FbTreeViewCtrl * owner = wxDynamicCast(GetOwner(), FbTreeViewCtrl);
		if (owner) owner->SetScrollPos(count);
	}
	return ok;
}

void FbLogModel::Append(wxString & result, size_t row)
{
	result << m_lines[row - 1];
	#ifdef __WXMSW__
	result << wxT("\r");
	#endif
	result << wxT("\n");
}

wxString FbLogModel::GetSelectedText()
{
	size_t count = m_lines.Count();
	if (count == 0 || m_position == 0) return wxEmptyString;

	wxString result;

	if (m_shift) {
		size_t min = m_shift < m_position ? m_shift : m_position;
		size_t max = m_shift > m_position ? m_shift : m_position;
		for (size_t row = min; row <= max; row++) {
			Append(result, row);
		}
	} else {
		if (m_ctrls.Count() == 0) {
			Append(result, m_position);
		} else {
			for (size_t row = 1; row <= count; row++) {
				if (m_ctrls.Index(row) == wxNOT_FOUND) continue;
				Append(result, row);
			}
		}
	}
	return result;
}

void FbLogModel::SelectAll(bool value)
{
	m_ctrls.Empty();
	if (value) {
		size_t count = m_lines.Count();
		m_ctrls.Alloc(count);
		for (size_t i = 1; i <= count; i++) {
			m_ctrls.Add(i);
		}
	}
}
