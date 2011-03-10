/////////////////////////////////////////////////////////////////////////////
// Name:        bzipcopy.zip
// Purpose:     Sample/Test for BZip streams
//              Just asks the user for a BZip file, which
//              it then decompresses and then recompresses to 
//              oldfilename + ".bz2"
//              If TEST_ARCHIVE is specified we read the file in
//              using wxFileSystem instead of a straight bzip stream
// Author:      Ryan Norton
// Modified by:
// Created:     12/12/2006
// RCS-ID:      $Id: bzipcopy.cpp,v 1.1 2006/12/12 17:20:06 ryannpcs Exp $
// Copyright:   (c) Ryan Norton
// Licence:     None (Public Domain)
/////////////////////////////////////////////////////////////////////////////

//#define TEST_ARCHIVE

#include <wx/wx.h>
#include <wx/wfstream.h>

#ifdef TEST_ARCHIVE
    #include <wx/filesys.h>
    #include <wx/fs_arc.h>
    #include <wx/fs_filter.h>
#endif

#include "bzipstream.h"


class wxBZipCopyApp : public wxApp
{
public:
	bool OnInit()
	{
#ifdef TEST_ARCHIVE
        wxFileSystem::AddHandler(new wxArchiveFSHandler);
        wxFileSystem::AddHandler(new wxFilterFSHandler);
#endif

		wxFileDialog f(NULL);
		if (f.ShowModal() == wxID_OK)
		{
#ifdef TEST_ARCHIVE
            wxFileSystem fs;
            wxFSFile* fsfile = fs.OpenFile(f.GetPath() + wxString("#bzip2:"));
            wxInputStream* zi = fsfile->GetStream();
#else
			wxInputStream* p1       = new wxFileInputStream(f.GetPath());
			wxBZipInputStream* zi   = new wxBZipInputStream(*p1);
#endif

            wxOutputStream *p2     = 
                new wxFileOutputStream(f.GetPath() + wxString(".bz2"));
			wxBZipOutputStream* zo = new wxBZipOutputStream(*p2);

			char buf[50000];
            int nRead;

            do
            {
                nRead = zi->Read(buf, 50000).LastRead();

                if(nRead > 0)
                    zo->Write(buf, nRead);

            }while(nRead == 50000);
			
#ifdef TEST_ARCHIVE
            delete fsfile;
#else
			delete zi;
            delete p1;
#endif
            zo->Close();
            p2->Close();
			delete zo;
            delete p2;
			wxMessageBox("Done!");
		}

		return false;
	}
};

IMPLEMENT_APP(wxBZipCopyApp);
