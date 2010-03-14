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
#include "FbDataModel.h"
#include "FbBookModel.h"
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
	    FbBookModel * model = new FbBookModel(dlg.GetPath());
	    m_dataview->ClearColumns();
	    m_dataview->AssociateModel(model);

		int flags = wxDATAVIEW_COL_RESIZABLE | wxCOL_SORTABLE | wxCOL_REORDERABLE;

		FbTitleRenderer *cr = new FbTitleRenderer;
		wxDataViewColumn *column = new wxDataViewColumn("title", cr, FbBookModel::COL_TITLE, 200, wxALIGN_LEFT, flags );
		m_dataview->AppendColumn( column );

		m_dataview->AppendTextColumn(_("author"), FbBookModel::COL_AUTHOR, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_LEFT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
		m_dataview->AppendTextColumn(_("rowid"),  FbBookModel::COL_ROWID,  wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
		m_dataview->AppendTextColumn(_("book"),   FbBookModel::COL_BOOKID, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
		m_dataview->AppendTextColumn(_("size"),   FbBookModel::COL_SIZE,   wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
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
	    m_dataview->ClearColumns();
	    m_dataview->AssociateModel(model);

		int flags = wxDATAVIEW_COL_RESIZABLE | wxCOL_SORTABLE | wxCOL_REORDERABLE;

		FbTitleRenderer *cr = new FbTitleRenderer;
		wxDataViewColumn *column = new wxDataViewColumn("title", cr, FbTreeModel::COL_TITLE, 200, wxALIGN_LEFT, flags );
		m_dataview->AppendColumn( column );

		m_dataview->AppendTextColumn(_("rowid"), FbTreeModel::COL_ROWID,  wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
		m_dataview->AppendTextColumn(_("book"),  FbTreeModel::COL_BOOKID, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
		m_dataview->AppendTextColumn(_("size"),  FbTreeModel::COL_SIZE,   wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
	};

}

