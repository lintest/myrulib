#include "FbDownloader.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/ptr_scpd.h>
#include "FbParams.h"
#include "polarssl/md5.h"
#include "MyRuLibApp.h"

#include <wx/sstream.h>
#include <wx/url.h>

void FbDownloadThread::Execute()
{

	wxThread * thread = new FbDownloadThread();
	thread->Create();
	if (url.GetProtocol().GetError() == wxPROTO_NOERR)
	thread->Run();
/*

	wxURL url(wxT("http://192.168.1.117/"));
	url.GetProtocol().SetTimeout(10);

	wxInputStream * httpStream = url.GetInputStream();

	if (url.GetProtocol().GetError() == wxPROTO_NOERR)
	{/*
		wxString res;
		wxStringOutputStream out_stream(&res);
		httpStream->Read(out_stream);
		wxMessageBox(res);
		wxThread * thread = new FbDownloadThread(0, httpStream);
		thread->Create();
		thread->Run();
	}
	else
	{
		wxMessageBox(_T("Unable to connect!"));
	}
*/
}

FbDownloadThread::FbDownloadThread()
{
	wxURL url(wxT("http://ya.ru/"));
	url.GetProtocol().SetTimeout(10);
	m_in = url.GetInputStream();
}

wxCriticalSection FbDownloadThread::sm_queue;

void * FbDownloadThread::Entry()
{
	wxString res;
	wxStringOutputStream out_stream(&res);
	m_in->Read(out_stream);
	wxMessageBox(res);
	return NULL;
}


/*
void * FbDownloadThread::Entry()
{

    	FbDownloadThread::Execute();
    	return NULL;



    wxCriticalSectionLocker enter(sm_queue);

//    wxString addr = FbParams::GetText(FB_LIBRUSEC_ADDR);
//    wxString path = wxString::Format(wxT("/b/%d/download"), m_id);

//	if ( FbParams::GetValue(FB_USE_PROXY) )
//		wxURL::SetDefaultProxy( FbParams::GetText(FB_PROXY_ADDR) );


    wxString addr = wxT("192.168.1.117");
    wxString path = wxT("/");

	wxHTTP get;
	get.SetTimeout(10);
 	while (!get.Connect(addr))
		wxSleep(3);

    wxInputStream * in = get.GetInputStream(path);
    if (!in) {
    	int error = get.GetError();
    	wxMessageBox(wxString::Format(_("%d"), error));
    	return NULL;
    }

    wxString tmpfile = wxFileName::CreateTempFileName(wxT("~"));
    wxFileOutputStream out(tmpfile);

    md5_context md5;
    md5_starts( &md5 );

    const size_t BUFSIZE = 1024;
    unsigned char buf[BUFSIZE];
    bool done;

    do {
        size_t len = in->Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);
		if (len) {
			md5_update( &md5, buf, (int) len );
			out.Write(buf, len);
		}
    } while (!done);

	wxDELETE(in);
	get.Close();

    wxString md5sum;

    unsigned char output[16];
    md5_finish( &md5, output );
    for (size_t i=0; i<16; i++) md5sum += wxString::Format(wxT("%02x"), output[i]);

	return NULL;
}
*/
