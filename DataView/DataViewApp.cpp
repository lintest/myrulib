/***************************************************************
 * Name:      DataViewApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "DataViewApp.h"
#include "DataViewMain.h"

IMPLEMENT_APP(DataViewApp);

bool DataViewApp::OnInit()
{
    DataViewFrame* frame = new DataViewFrame(0L);
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();

    return true;
}
