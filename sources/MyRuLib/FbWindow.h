#ifndef __FBWINDOW_H__
#define __FBWINDOW_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/artprov.h>

class FbDialog : public wxDialog
{
public:
	FbDialog() {}

	FbDialog(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER,
		const wxString& name = wxFrameNameStr
	);

	bool Create(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER,
		const wxString& name = wxFrameNameStr
	);

protected:
	static wxString GetCommandSQL(const wxString &table);
	void Assign(long winid, int param, bool write);
};

class FbToolBar : public wxToolBar
{
public:
	wxToolBarToolBase * AddTool(int itemid, const wxString& text, const wxArtID& art, const wxString& help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL) {
		return wxToolBar::AddTool(itemid, text, wxArtProvider::GetBitmap(art, wxART_TOOLBAR), help);
	}

	wxToolBarToolBase * AddTool(int itemid, const wxString& text, const wxBitmap& bitmap, const wxString& help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL) {
		return wxToolBar::AddTool(itemid, text, bitmap, help);
	}

private:
	void OnSubmenu(wxCommandEvent & event) { }
	DECLARE_CLASS(FbToolBar)
	DECLARE_EVENT_TABLE()
};

#endif // __FBWINDOW_H__
