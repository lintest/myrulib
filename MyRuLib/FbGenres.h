#ifndef FBGENRES_H
#define FBGENRES_H

#include <wx/wx.h>

class FbGenres {
    public:
        static wxString Char(const wxString &code);
        static wxString Name(const wxString &letter);
        static wxString DecodeList(const wxString &genres);
};

#endif //FBGENRES_H

