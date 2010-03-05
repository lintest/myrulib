/***************************************************************
 * Name:      DataViewMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef DATAVIEWMAIN_H
#define DATAVIEWMAIN_H

#include <wx/wx.h>
#include "GUIFrame.h"

class DataViewFrame: public GUIFrame
{
    public:
        DataViewFrame(wxFrame *frame);
        ~DataViewFrame();
    private:
        virtual void OnClose(wxCloseEvent& event);
        virtual void OnQuit(wxCommandEvent& event);
        virtual void OnAbout(wxCommandEvent& event);
        virtual void OnOpen(wxCommandEvent& event);
};

#endif // DATAVIEWMAIN_H
