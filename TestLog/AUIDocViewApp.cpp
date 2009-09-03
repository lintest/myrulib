#include "AUIDocViewApp.h"
#include "AUIDocViewMainFrame.h"
#include <wx/image.h>

IMPLEMENT_APP(AUIDocViewApp)

bool AUIDocViewApp::OnInit()
{
	srand(time(NULL));

	wxImage::AddHandler(new wxXPMHandler);
	wxImage::AddHandler(new wxPNGHandler);

	AUIDocViewMainFrame * frame = new AUIDocViewMainFrame(NULL);
	SetTopWindow(frame);
	frame->Centre();
	frame->Show();
	return true;
}

int AUIDocViewApp::OnExit()
{
	return wxApp::OnExit();
}