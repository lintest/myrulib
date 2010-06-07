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
		FbCollection * GetCollection() { return m_collection; }
		void OpenCollection(const wxString & filename);
	private:
		FbCollection * m_collection;
};

DECLARE_APP(DataViewApp)

#endif // DATAVIEWAPP_H
