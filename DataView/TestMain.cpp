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
	    m_dataview->AssociateModel(model);
	};
}

void DataViewFrame::OnOpenTree(wxCommandEvent &event)
{
/*
	wxDataViewTreeStore * model = new wxDataViewTreeStore;
	wxDataViewItem parent = model->AppendItem( wxDataViewItem(NULL), "test" );
	parent = model->AppendItem( parent, "test2" );
	parent = model->AppendItem( parent, "test3" );
    m_dataview->AssociateModel(model);
	return;

*/
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
	};

}

