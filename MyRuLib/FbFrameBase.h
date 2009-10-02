#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "BooksPanel.h"
#include "FbBookEvent.h"
#include "FbParams.h"

class FbFrameBase : public wxAuiMDIChildFrame
{
public:
	FbFrameBase();
	FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& WXUNUSED(name)) { return NULL; };
	static FbListMode GetListMode(FbParamKey key);
	static void SetListMode(FbParamKey key, FbListMode mode);
protected:
	virtual void CreateControls() {};
	virtual wxMenuBar * CreateMenuBar();
protected:
    BooksPanel m_BooksPanel;
private:
	void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
	void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
    void OnExternal(wxCommandEvent& event);
    void OnSubmenu(wxCommandEvent& event);
    void OnAppendBook(FbBookEvent& event);
    void OnAppendAuthor(wxCommandEvent& event);
    void OnAppendSequence(wxCommandEvent& event);
    void OnEmptyBooks(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

class FbFrameBaseThread: public wxThread
{
	public:
        FbFrameBaseThread(wxWindow * frame, FbListMode mode)
			:m_frame(frame), m_mode(mode) {};
		wxString GetSQL(const wxString & condition);
	protected:
		void EmptyBooks();
		void FillBooks(wxSQLite3ResultSet &result);
		static wxCriticalSection sm_queue;
        wxWindow * m_frame;
	private:
		void CreateList(wxSQLite3ResultSet &result);
		void CreateTree(wxSQLite3ResultSet &result);
	private:
        FbListMode m_mode;
};

class FbThreadSkiper
{
	public:
		FbThreadSkiper(): m_number(0) {};
		bool Skipped(const int number) { return number != m_number; };
		int NewNumber() { return ++m_number; };
	private:
		wxCriticalSection m_queue;
		int m_number;
};

#endif //__FBFRAMEBASE_H__

