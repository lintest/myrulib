#include "FbManager.h"

wxString BookInfo::MakeLower(wxString & data)
{
#if defined(__WIN32__)
	int len = data.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, data.c_str());
	CharLower(buf);
	data = buf;
	delete [] buf;
#else
	data.MakeLower();
#endif
	return data;
}

wxString BookInfo::MakeUpper(wxString & data)
{
#if defined(__WIN32__)
	int len = data.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, data.c_str());
	CharUpper(buf);
	data = buf;
	delete [] buf;
#else
	data.MakeUpper();
#endif
	return data;
}

