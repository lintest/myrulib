/***************************************************************
 * Name:      DataViewApp.h
 * Purpose:   Defines Application Class
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef DATAVIEWAPP_H
#define DATAVIEWAPP_H

#include <wx/wx.h>
#include <wx/app.h>

class DataViewApp : public wxApp
{
    public:
        virtual bool OnInit();
};

DECLARE_APP(DataViewApp)

#endif // DATAVIEWAPP_H
