#include "FbWindow.h"
#include "FbParams.h"
#include "FbMainMenu.h"
#include "controls/FbChoiceCtrl.h"
#include "controls/FbComboBox.h"
#include <wx/combo.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>

//-----------------------------------------------------------------------------
//  FbDialog
//-----------------------------------------------------------------------------

FbDialog::FbDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
	:wxDialog( parent, id, title, pos, size, style, name )
{
	SetFont( FbParams(FB_FONT_DLG) );
}

bool FbDialog::Create( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
	bool res = wxDialog::Create( parent, id, title, pos, size, style, name );
	SetFont( FbParams(FB_FONT_DLG) );
	return res;
}

void FbDialog::Assign(long winid, int param, bool write)
{
	wxWindow * window = FindWindow(winid);
	if (window == NULL) {
		wxLogError(wxT("Control not found: %d"), winid);
		return;
	} else if (wxTextCtrl * control = wxDynamicCast(window, wxTextCtrl)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue(FbParams(param));
	} else if (wxCheckBox * control = wxDynamicCast(window, wxCheckBox)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue(FbParams(param));
	} else if (wxComboBox * control = wxDynamicCast(window, wxComboBox)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue(FbParams(param));
	} else if (wxComboCtrl * control = wxDynamicCast(window, wxComboCtrl)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue(FbParams(param));
	} else if (FbChoiceStr * control = wxDynamicCast(window, FbChoiceStr)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue(FbParams(param));
	} else if (FbCustomCombo * control = wxDynamicCast(window, FbCustomCombo)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue(FbParams(param));
	} else if (wxSpinCtrl * control = wxDynamicCast(window, wxSpinCtrl)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue((int)FbParams(param));
	} else if (FbChoiceInt * control = wxDynamicCast(window, FbChoiceInt)) {
		if (write) FbParams(param) = control->GetValue(); else control->SetValue((int)FbParams(param));
	} else if (wxRadioBox * control = wxDynamicCast(window, wxRadioBox)) {
		if (write) FbParams(param) = control->GetSelection(); else control->SetSelection(FbParams(param));
	} else if (wxFontPickerCtrl * control = wxDynamicCast(window, wxFontPickerCtrl)) {
		if (write)
			FbParams(param) = control->GetSelectedFont().GetNativeFontInfoDesc();
		else
			control->SetSelectedFont(FbParams(param));
	} else if (wxColourPickerCtrl * control = wxDynamicCast(window, wxColourPickerCtrl)) {
		if (write) {
			wxColor colour = control->GetColour();
			FbParams(param) = colour.Red() * 0x10000 + colour.Green() * 0x100 + colour.Blue();
		} else {
			wxColor colour(wxString::Format(wxT("#%06x"), (int)FbParams(param))); 
			control->SetColour(colour);
		}
	} else wxLogError(wxT("Control not found: %d"), winid);
}

