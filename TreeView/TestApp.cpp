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

IMPLEMENT_APP(DataViewApp);

bool DataViewApp::OnInit()
{
	wxLog::SetVerbose(true);
    DataViewFrame* frame = new DataViewFrame(0L);
    frame->Show();

    return true;
}

