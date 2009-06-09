#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>

class FbGenres {
public:
    static wxString Char(wxString &code);
    static wxString Code(wxChar &letter);
    static wxString Name(wxChar &letter);
};

#endif //FBGENRES_H

