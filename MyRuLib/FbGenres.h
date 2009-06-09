#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>

struct GenreStruct {
	wxChar letter;
	wxString code;
	wxString name;
};

extern const GenreStruct all_genres[];

#endif //FBGENRES_H

