#include "FbLocale.h"
#include <wx/wfstream.h>

bool FbLocale::Init(const wxFileName &filename)
{
#ifdef FB_INCLUDE_LOCALE

	switch (GetSystemLanguage()) {
		case wxLANGUAGE_RUSSIAN: {
			#include "ru.inc"
			Save(filename, locale_binary_file, sizeof(locale_binary_file));
		} break;
		case wxLANGUAGE_UKRAINIAN: {
			#include "uk.inc"
			Save(filename, locale_binary_file, sizeof(locale_binary_file));
		} break;
		case wxLANGUAGE_BELARUSIAN: {
			#include "be.inc"
			Save(filename, locale_binary_file, sizeof(locale_binary_file));
		} break;
	}

	AddCatalogLookupPathPrefix(filename.GetPath());

#endif

    return wxLocale::Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);
}

void FbLocale::Save(const wxFileName &filename, const unsigned char *data, size_t size)
{
	wxFileOutputStream out(filename.GetFullPath());
	out.Write(data, size);
}

