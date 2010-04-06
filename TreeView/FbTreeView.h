#ifndef __FBTREEVIEW_H__
#define __FBTREEVIEW_H__

#include <wx/treectrl.h>
#include <wx/control.h>
#include <wx/pen.h>
#include <wx/listctrl.h>
#include <wx/containr.h>

#include "FbTreeModel.h"

#define DEFAULT_COL_WIDTH 100

extern WXDLLEXPORT const wxChar* FbTreeViewCtrlNameStr;

class FbTreeViewHeaderWindow;

class FbTreeViewMainWindow;

class WXDLLEXPORT FbTreeViewCtrl : public wxControl
{
	public:
		FbTreeViewCtrl()
			: m_header_win(0), m_main_win(0)
		{}

		FbTreeViewCtrl(wxWindow *parent, wxWindowID id = -1,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = FbTreeViewCtrlNameStr )
			: m_header_win(0), m_main_win(0)
		{
			Create(parent, id, pos, size, style);
		}

		virtual ~FbTreeViewCtrl() {}

		bool Create(wxWindow *parent, wxWindowID id = -1,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxTR_DEFAULT_STYLE,
					const wxValidator &validator = wxDefaultValidator,
					const wxString& name = FbTreeViewCtrlNameStr );

		void Refresh(bool erase=TRUE, const wxRect* rect=NULL);

		// overridden base class virtuals
		virtual bool SetBackgroundColour (const wxColour& colour);

		virtual bool SetForegroundColour (const wxColour& colour);

		virtual wxSize DoGetBestSize() const;

        virtual bool SetFont(const wxFont& font);

        void AddColumn (size_t model_column,
                        const wxString& text = wxEmptyString,
                        int width = DEFAULT_COL_WIDTH,
                        int flag = wxALIGN_LEFT);

		void AssignModel(FbTreeModel * model);

		FbTreeViewHeaderWindow* GetHeaderWindow() const
			{ return m_header_win; }

		FbTreeViewMainWindow* GetMainWindow() const
			{ return m_main_win; }

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
        WX_DECLARE_CONTROL_CONTAINER();
		DECLARE_DYNAMIC_CLASS(FbTreeViewCtrl)
		DECLARE_EVENT_TABLE()
};

#endif // __FBTREEVIEW_H__

