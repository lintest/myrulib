#ifndef __FBLOGMODEL_H__
#define __FBLOGMODEL_H__

#include "FbTreeModel.h"
#include "FbTreeView.h"
#include "FbMenu.h"

class FbLogViewCtrl
	: public FbTreeViewCtrl
{
private:
	void Init() {}

private:
	class ContextMenu: public FbMenu {
		public: ContextMenu();
	};

	void OnContextMenu(wxTreeEvent& event);
	
	void OnCopy(wxCommandEvent & event);

	void OnSelect(wxCommandEvent & event) {
		SelectAll(true);
	}
	void OnUnselect(wxCommandEvent & event) {
		SelectAll(false);
	}

	void OnEnableUI(wxUpdateUIEvent & event) {
		event.Enable(GetModel() && GetModel()->GetRowCount());
	}

	void OnDisableUI(wxUpdateUIEvent & event) {
		event.Enable(false);
	}

	DECLARE_CLASS(FbLogViewCtrl)
	DECLARE_EVENT_TABLE()
};

class FbLogData: public FbModelData
{
	public:
		FbLogData(const wxString & text)
			: m_text(text) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return m_text; }
		virtual FbModelData * Clone() const
			 { return new FbLogData(m_text); }
	private:
		wxString m_text;
		DECLARE_CLASS(FbLogData);
};

class FbLogModel : public FbListModel
{
	public:
		wxString GetSelectedText();
		virtual void SelectAll(bool value);
		virtual void Append(FbModelData * data) {}
		virtual void Replace(FbModelData * data) {}
		virtual void Delete() {}
		bool Update();
	public:
		virtual size_t GetRowCount() const
			{ return m_lines.Count(); }
		virtual FbModelItem GetCurrent()
			{ return GetData(m_position); };
	protected:
		virtual FbModelItem DoGetData(size_t row, int &level);
		void Append(wxString & result, size_t row);
	private:
		wxArrayString m_lines;
		DECLARE_CLASS(FbLogData);
};

#endif // __FBLOGMODEL_H__
