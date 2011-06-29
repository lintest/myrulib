#ifndef __FBTREEVIEW_H__
#define __FBTREEVIEW_H__

#include <wx/control.h>
#include <wx/pen.h>
#include <wx/listbase.h>
#include <wx/treebase.h>

#include "FbTreeModel.h"

#define fbTR_HRULES     0x0001
#define fbTR_VRULES     0x0002
#define fbTR_MULTIPLE   0x0004
#define fbTR_CHECKBOX   0x0008
#define fbTR_NO_HEADER  0x0010
#define fbTR_DIRECTORY  0x0020

#define fbTR_DEFAULT_STYLE (fbTR_VRULES)

#define DEFAULT_COL_WIDTH 100

extern WXDLLEXPORT const wxChar* FbTreeViewCtrlNameStr;

class FbTreeViewHeaderWindow;

class FbTreeViewMainWindow;

class WXDLLEXPORT FbTreeViewCtrl : public wxControl
{
	public:
		FbTreeViewCtrl()
			: m_header_win(0), m_main_win(0), m_position(0)
		{}

		FbTreeViewCtrl(wxWindow *parent, wxWindowID id = -1,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = fbTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = FbTreeViewCtrlNameStr )
			: m_header_win(0), m_main_win(0), m_position(0)
		{
			Create(parent, id, pos, size, style);
		}

		virtual ~FbTreeViewCtrl() {}

		bool Create(wxWindow *parent, wxWindowID id = -1,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = fbTR_DEFAULT_STYLE,
					const wxValidator &validator = wxDefaultValidator,
					const wxString& name = FbTreeViewCtrlNameStr );

		void Refresh(bool erase=TRUE, const wxRect* rect=NULL);
		
		void SetScrollPos(int position) { m_position = position; }
		int GetScrollPos() { return m_position; }

		// overridden base class virtuals
		virtual bool SetBackgroundColour (const wxColour& colour);

		virtual bool SetForegroundColour (const wxColour& colour);

		virtual void SetFocus();

		virtual bool SetFont(const wxFont& font);

		void AddColumn (size_t model_column,
						const wxString& text = wxEmptyString,
						int width = DEFAULT_COL_WIDTH,
						int flag = wxALIGN_LEFT,
						int fixed = 0);

		void EmptyColumns();

		void GetColumns(wxArrayInt &columns) const;

		void AssignModel(FbModel * model);

		FbModel * GetModel() const;

		FbTreeViewHeaderWindow* GetHeaderWindow() const
			{ return m_header_win; }

		FbTreeViewMainWindow* GetMainWindow() const
			{ return m_main_win; }

		int GetRowHeight();

		FbModelItem GetCurrent() const;
		wxString GetCurrentText() const;

		bool FindAt(const wxPoint &point, bool select = false);

		wxString GetText();
		void SelectAll(bool value);
		void Append(FbModelData * data);
		void Replace(FbModelData * data);
		void Delete();

	public:
#ifdef _MYRULIB
		virtual FbMasterInfo GetInfo() const;
#endif // _MYRULIB
		virtual int GetBook() const;
	protected:
		// header window, responsible for column visualization and manipulation
		FbTreeViewHeaderWindow* m_header_win;

		// main window, the "true" tree ctrl
		FbTreeViewMainWindow* m_main_win;

		void DoHeaderLayout();
		void OnSize(wxSizeEvent& event);

	public:
		void SetSortedColumn(int column);
		int GetSortedColumn();

	private:
		int m_position;
		DECLARE_DYNAMIC_CLASS(FbTreeViewCtrl)
		DECLARE_EVENT_TABLE()
};

#endif // __FBTREEVIEW_H__

