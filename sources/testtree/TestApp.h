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

class FbCollection;

class DataViewApp : public wxApp
{
    public:
		DataViewApp(): m_collection(NULL) {}
        virtual bool OnInit();
	private:
		FbCollection * m_collection;
};

DECLARE_APP(DataViewApp)

#endif // DATAVIEWAPP_H
