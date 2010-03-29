/***************************************************************
 * Name:      DataViewMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "TestMain.h"
#include "TestApp.h"
#include "FbDataView.h"
#include "FbModelData.h"
#include "FbListModel.h"
#include "FbTreeModel.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}


DataViewFrame::DataViewFrame(wxFrame *frame)
    : GUIFrame(frame)
{
#if wxUSE_STATUSBAR
    m_statusbar->SetStatusText(_("Hello Code::Blocks user!"), 0);
    m_statusbar->SetStatusText(wxbuildinfo(short_f), 1);
#endif
}

DataViewFrame::~DataViewFrame()
{
}

void DataViewFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void DataViewFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void DataViewFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void DataViewFrame::OnOpenList(wxCommandEvent &event)
{
	wxFileDialog dlg (
		this,
		_("Select archive to add to the library"),
		wxEmptyString,
		wxEmptyString,
		wxT("*.db"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
	    FbListModel * model = new FbListModel(dlg.GetPath());
	    m_dataview->AssociateModel(model);

	    while (m_dataview->GetColumnCount()>1) m_dataview->DeleteColumn(m_dataview->GetColumn(1));

        int flags = wxDATAVIEW_COL_RESIZABLE;
        m_dataview->AppendTextColumn(_("author"), 4, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("rowid"),  1, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("book"),   2, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("size"),   3, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        wxDynamicCast(m_dataview, FbDataViewCtrl)->Resize();
	};
}

void DataViewFrame::OnOpenTree(wxCommandEvent &event)
{
	wxFileDialog dlg (
		this,
		_("Select archive to add to the library"),
		wxEmptyString,
		wxEmptyString,
		wxT("*.db"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
        FbTreeModel * model = new FbTreeModel(dlg.GetPath());
	    m_dataview->AssociateModel(model);

	    while (m_dataview->GetColumnCount()>1) m_dataview->DeleteColumn(m_dataview->GetColumn(1));

        int flags = wxDATAVIEW_COL_RESIZABLE;
        m_dataview->AppendTextColumn(_("rowid"),  1, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("book"),   2, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("size"),   3, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        wxDynamicCast(m_dataview, FbDataViewCtrl)->Resize();
	};

}

