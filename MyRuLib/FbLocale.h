#ifndef __FBLOCALE_H__
#define __FBLOCALE_H__

#include <wx/intl.h>
#include <wx/filename.h>

class FbLocale: public wxLocale
{
	public:
		bool Init(const wxFileName &filename);
	private:
		void Save(const wxFileName &filename, const unsigned char *data, size_t size);
};

#endif // __FBLOCALE_H__
