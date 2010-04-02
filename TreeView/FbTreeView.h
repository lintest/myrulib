#ifndef __FBTREEVIEW_H__
#define __FBTREEVIEW_H__

#include <wx/treectrl.h>
#include <wx/control.h>
#include <wx/pen.h>
#include <wx/listctrl.h>

#define DEFAULT_COL_WIDTH 100

class wxTreeListItem
{

};

class wxTreeListModel
{
    public:
        enum ModelType {
            MT_LIST,
            MT_TREE,
        };
    public:
        wxTreeListModel() {};
        virtual ModelType GetType() = 0;
        virtual unsigned int GetCount() = 0;
};

extern WXDLLEXPORT const wxChar* wxTreeListCtrlNameStr;

class wxTreeListHeaderWindow;

class wxTreeListMainWindow;

class WXDLLEXPORT wxTreeListCtrl : public wxControl
{
	public:
		wxTreeListCtrl()
			: m_header_win(0), m_main_win(0)
		{}

		wxTreeListCtrl(wxWindow *parent, wxWindowID id = -1,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = wxTreeListCtrlNameStr )
			: m_header_win(0), m_main_win(0)
		{
			Create(parent, id, pos, size, style);
		}

		virtual ~wxTreeListCtrl() {}

		bool Create(wxWindow *parent, wxWindowID id = -1,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxTR_DEFAULT_STYLE,
					const wxValidator &validator = wxDefaultValidator,
					const wxString& name = wxTreeListCtrlNameStr );

		void Refresh(bool erase=TRUE, const wxRect* rect=NULL);

		void SetFocus();

		// overridden base class virtuals
		virtual bool SetBackgroundColour (const wxColour& colour);
		virtual bool SetForegroundColour (const wxColour& colour);

		wxTreeListHeaderWindow* GetHeaderWindow() const
			{ return m_header_win; }

		wxTreeListMainWindow* GetMainWindow() const
			{ return m_main_win; }

		virtual wxSize DoGetBestSize() const;

        virtual bool SetFont(const wxFont& font);

        void AddColumn (const wxString& text,
                        unsigned int model_column,
                        int width = DEFAULT_COL_WIDTH,
                        int flag = wxALIGN_LEFT);

	protected:
		// header window, responsible for column visualization and manipulation
		wxTreeListHeaderWindow* m_header_win;

		// main window, the "true" tree ctrl
		wxTreeListMainWindow* m_main_win;

		void DoHeaderLayout();
		void OnSize(wxSizeEvent& event);

	public:
		void SetSortedColumn(int column);
		int GetSortedColumn();

	private:
		DECLARE_EVENT_TABLE()
		DECLARE_DYNAMIC_CLASS(wxTreeListCtrl)

};

#endif // __FBTREEVIEW_H__

