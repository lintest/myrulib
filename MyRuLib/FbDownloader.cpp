#include "FbDownloader.h"
#include <wx/protocol/http.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/ptr_scpd.h>
#include "FbParams.h"
#include "polarssl/md5.h"
#include "MyRuLibApp.h"

#include <wx/sstream.h>

void FbDownloadThread::Execute()
{
	wxHTTP get;
	get.SetTimeout(10); // 10 seconds of timeout instead of 10 minutes ...

	// this will wait until the user connects to the internet. It is important in case of dialup (or ADSL) connections
	while (!get.Connect(_T("192.168.1.117")))  // only the server, no pages here yet ...
		wxSleep(5);

	wxApp::IsMainLoopRunning(); // should return true

	wxInputStream *httpStream = get.GetInputStream(_T("/"));

	// wxLogVerbose( wxString(_T(" GetInputStream: ")) << get.GetResponse() << _T("-") << ((resStream)? _T("OK ") : _T("FAILURE ")) << get.GetError() );

	if (get.GetError() == wxPROTO_NOERR)
	{
		wxString res;
		wxStringOutputStream out_stream(&res);
		httpStream->Read(out_stream);

		wxMessageBox(res);
		// wxLogVerbose( wxString(_T(" returned document length: ")) << res.Length() );
	}
	else
	{
		wxMessageBox(_T("Unable to connect!"));
	}

	wxDELETE(httpStream);
	get.Close();
}

wxCriticalSection FbDownloadThread::sm_queue;

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
