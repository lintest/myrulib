#include "FbLogModel.h"
#include "FbLogStream.h"
#include "FbTreeView.h"

#define MAXIMUM_LOG_ROW_COUNT 1024

IMPLEMENT_CLASS(FbLogData, FbModelData)

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

