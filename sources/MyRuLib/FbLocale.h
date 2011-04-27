#ifndef __FBLOCALE_H__
#define __FBLOCALE_H__

#include <wx/intl.h>
#include <wx/filename.h>

class FbChoiceInt;

class FbLocale: public wxLocale
{
	public:
        virtual bool Init(int language = wxLANGUAGE_DEFAULT, int flags = wxLOCALE_LOAD_DEFAULT);
        static int MenuToLang(wxWindowID id);
        static void Fill(FbChoiceInt * choise, int value);
	private:
		bool LoadResource(const wxLanguageInfo * info, const wxString & filename);
		bool Save(const wxFileName &filename, const void *data, size_t size);
        wxFileName GetLocaleFilename();
        int m_language;
};

#endif // __FBLOCALE_H__
