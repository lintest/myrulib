/***************************************************************
 * Name:      DataViewApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "TestApp.h"
#include "TestMain.h"
#include "FbCollection.h"

IMPLEMENT_APP(DataViewApp);

bool DataViewApp::OnInit()
{
    DataViewFrame* frame = new DataViewFrame(0L);
    frame->Show();

    return true;
}

void DataViewApp::OpenCollection(const wxString & filename)
{
	wxDELETE(m_collection);
	m_collection = new FbCollection(filename);
}
